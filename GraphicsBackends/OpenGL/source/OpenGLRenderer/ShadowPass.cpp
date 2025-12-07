#include "ChaiEngine/MaterialSystem.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include <OpenGLRenderer/UniformManager.h>
#include <OpenGLRenderer/ShadowPass.h>

namespace chai::brew
{
    ShadowPass::ShadowPass() {}
    ShadowPass::~ShadowPass() {}

    void ShadowPass::createShadowFBO()
    {
        if (!m_backend)
            return;

        auto* openGLBackend = static_cast<brew::OpenGLBackend*>(m_backend);

        for (int i = 0; i < 16; i++) {
            LightShadowData& lightData = m_lightShadowData.emplace_back();

            for (int c = 0; c < NUM_CASCADES; c++) {
                CascadeData& cascade = lightData.cascades[c];

                glGenFramebuffers(1, &cascade.shadowFBO);
                glBindFramebuffer(GL_FRAMEBUFFER, cascade.shadowFBO);

                glGenTextures(1, &cascade.shadowTex);
                glBindTexture(GL_TEXTURE_2D, cascade.shadowTex);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
                             SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

                // Texture Parameters (Using GL_LINEAR for PCF)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                // Clamp to border with max depth—samples outside the map are "lit"
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                       GL_TEXTURE_2D, cascade.shadowTex, 0);

                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);

                GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                if (status != GL_FRAMEBUFFER_COMPLETE) {
                    printf("Shadow FBO incomplete for light %d cascade %d: 0x%x\n",
                           i, c, status);
                }
            }
            lightData.lightProj = createUniform<ShadowPassUniforms>();
            lightData.lightProj->setValue(ShadowPassUniforms{Mat4::identity()});
            openGLBackend->getUniformManager().buildUniforms({lightData.lightProj.get()});
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowPass::destroyShadowFBO()
    {
        for (auto& lightData : m_lightShadowData) {
            for (int c = 0; c < NUM_CASCADES; c++) {
                if (lightData.cascades[c].shadowTex) {
                    glDeleteTextures(1, &lightData.cascades[c].shadowTex);
                }
                if (lightData.cascades[c].shadowFBO) {
                    glDeleteFramebuffers(1, &lightData.cascades[c].shadowFBO);
                }
            }
        }
        m_lightShadowData.clear();
    }

    void ShadowPass::setup(void* backend)
    {
        m_backend = backend;
        auto* openGLBackend = static_cast<OpenGLBackend*>(backend);

        m_shaderAsset = MaterialSystem::instance().getShadowShader();
        m_shaderProgram = openGLBackend->getShaderManager().compileShaderFromAsset(m_shaderAsset);
        m_shaderData = openGLBackend->getShaderManager().getShaderData(m_shaderProgram);

        createShadowFBO();
    }

    void ShadowPass::resize(int width, int height)
    {
        // probably don't really need to keep this
        m_width = width;
        m_height = height;
    }

    void ShadowPass::calculateCascadeSplits(float nearPlane, float farPlane)
    {
        float lambda = 0.75f;

        // Use a larger effective near plane for shadow calculations
        float shadowNear = std::max<float>(nearPlane, 1.0f);  // At least 1 unit
        float ratio = farPlane / shadowNear;

        m_cascadeSplits[0] = shadowNear;

        for (int i = 1; i <= NUM_CASCADES; i++) {
            float p = (float)i / (float)NUM_CASCADES;
            float log = shadowNear * std::pow(ratio, p);
            float uniform = shadowNear + (farPlane - shadowNear) * p;
            m_cascadeSplits[i] = lambda * log + (1.0f - lambda) * uniform;
        }

        /*printf("Cascade splits: %f, %f, %f, %f, %f\n",
               m_cascadeSplits[0], m_cascadeSplits[1], m_cascadeSplits[2],
               m_cascadeSplits[3], m_cascadeSplits[4]);*/
    }

    std::vector<Vec3> ShadowPass::getFrustumCornersWorldSpace(const Mat4& viewProj)
    {
        Mat4 invViewProj = inverse(viewProj);

        std::vector<Vec3> corners;
        corners.reserve(8);

        for (int x = 0; x < 2; ++x) {
            for (int y = 0; y < 2; ++y) {
                for (int z = 0; z < 2; ++z) {
                    Vec4 pt = invViewProj * Vec4(
                        2.0f * x - 1.0f,
                        2.0f * y - 1.0f,
                        2.0f * z - 1.0f,
                        1.0f
                    );
                    corners.push_back(Vec3(pt.x, pt.y, pt.z) / pt.w);
                }
            }
        }

        return corners;
    }

    Mat4 ShadowPass::calculateLightViewProjForCascade(
    const Vec3& lightDir,
    const Mat4& cameraView,
    const Mat4& cameraProj,
    float nearSplit,
    float farSplit)
{
    float aspect = cameraProj[1][1] / cameraProj[0][0];
    float fov = 2.0f * std::atan(1.0f / cameraProj[1][1]);

    Mat4 cascadeProj = perspective(fov, aspect, nearSplit, farSplit);
    Mat4 cascadeViewProj = cascadeProj * cameraView;

    std::vector<Vec3> frustumCorners = getFrustumCornersWorldSpace(cascadeViewProj);

    // Calculate frustum center
    Vec3 center(0.0f);
    for (const auto& corner : frustumCorners) {
        center = center + corner;
    }
    center = center / 8.0f;

    // Calculate cascade radius for stabilization
    float radius = 0.0f;
    for (const auto& corner : frustumCorners) {
        float dist = length(corner - center);
        radius = std::max<float>(radius, dist);
    }
    radius = std::ceil(radius);

    // Texel snapping factor
    float texelsPerUnit = (float)SHADOW_MAP_SIZE / (radius * 2.0f);

    // Create light view
    Vec3 up = Vec3(0, 1, 0);
    if (std::abs(dot(lightDir, up)) > 0.99f) {
        up = Vec3(1, 0, 0);
    }

    Mat4 lightView = lookAt(center - lightDir * radius, center, up);

    // Snap to texel grid to prevent shimmering
    Vec4 shadowOrigin = lightView * Vec4(0, 0, 0, 1);
    shadowOrigin = shadowOrigin * texelsPerUnit;
    Vec4 roundedOrigin = Vec4(
        std::round(shadowOrigin.x),
        std::round(shadowOrigin.y),
        std::round(shadowOrigin.z),
        shadowOrigin.w
    );
    Vec4 offset = (roundedOrigin - shadowOrigin) / texelsPerUnit;

    // Apply offset to light view
    lightView[3][0] += offset.x;
    lightView[3][1] += offset.y;

    // Use sphere bounds for stable ortho (doesn't change with camera rotation)
    float minZ = -radius * 4.0f;  // Extend behind to catch shadow casters
    float maxZ = radius * 4.0f;

    Mat4 lightProj = ortho(-radius, radius, -radius, radius, -maxZ, -minZ);

    return lightProj * lightView;
}

    void ShadowPass::execute(
        void* backend,
        const std::vector<LightInfo>& lights,
        const std::vector<SortedDrawCommand>& draws,
        const Mat4& cameraView,
        const Mat4& cameraProj,
        float nearPlane,
        float farPlane)
    {
        m_lights = lights;

        if (m_shaderProgram == 0) return;

        auto* openGLBackend = static_cast<OpenGLBackend*>(backend);

        // Calculate cascade splits
        calculateCascadeSplits(nearPlane, farPlane);

        glUseProgram(m_shaderProgram);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        auto* shadowShaderAsset = AssetManager::instance().get<ShaderAsset>(m_shaderAsset);

        for (size_t i = 0; i < lights.size(); ++i) {
            auto& light = lights[i];
            auto& lightData = m_lightShadowData.at(i);

            if (!light.castsShadow) continue;

            float lightType = light.light.positionAndType.w;
            if (lightType != 0.0f) continue;  // CSM only for directional lights

            Vec3 lightDir = normalize(Vec3(
                light.light.directionAndRange.x,
                light.light.directionAndRange.y,
                light.light.directionAndRange.z
            ));

            // Render each cascade
            for (int c = 0; c < NUM_CASCADES; c++) {
                CascadeData& cascade = lightData.cascades[c];

                // Calculate light view-proj for this cascade
                cascade.lightViewProj = calculateLightViewProjForCascade(
                    lightDir, cameraView, cameraProj,
                    m_cascadeSplits[c], m_cascadeSplits[c + 1]
                );
                cascade.splitDepth = m_cascadeSplits[c + 1];

                // Update uniform buffer
                lightData.lightProj->setValue(ShadowPassUniforms{cascade.lightViewProj});
                openGLBackend->getUniformManager().updateUniform(*lightData.lightProj);

                auto* ub = openGLBackend->getUniformManager().getUniformBufferData(*lightData.lightProj);
                if (ub) {
                    glBindBufferBase(GL_UNIFORM_BUFFER, 2, ub->ubo);
                }

                // Bind cascade FBO
                glBindFramebuffer(GL_FRAMEBUFFER, cascade.shadowFBO);
                glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
                glClear(GL_DEPTH_BUFFER_BIT);

                // Render scene
                for (const auto& draw : draws) {
                    auto& cmd = draw.command;

                    OpenGLMeshData* meshData = openGLBackend->getMeshManager().getOrCreateMeshData(cmd.mesh);
                    if (!meshData || !meshData->isUploaded) continue;

                    GLuint vao = openGLBackend->getMeshManager().getOrCreateVAO(
                        meshData, m_shaderProgram, shadowShaderAsset);
                    if (vao == 0) continue;

                    glBindVertexArray(vao);
                    openGLBackend->updatePerDrawUniforms(cmd, m_shaderData);

                    if (meshData->indexCount > 0) {
                        glDrawElements(GL_TRIANGLES, cmd.indexCount, GL_UNSIGNED_INT,
                                       (void*)(cmd.indexOffset * sizeof(uint32_t)));
                    } else {
                        glDrawArrays(GL_TRIANGLES, 0, meshData->vertexCount);
                    }
                }
            }
        }

        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}