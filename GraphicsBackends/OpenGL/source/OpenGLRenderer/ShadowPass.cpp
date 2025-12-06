#include "ChaiEngine/MaterialSystem.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include <OpenGLRenderer/UniformManager.h>
#include <OpenGLRenderer/ShadowPass.h>

#include <corecrt_io.h>

namespace chai
{

    ShadowPass::~ShadowPass()
    {}

    void ShadowPass::createShadowFBO()
    {
        auto* openGLBackend = static_cast<brew::OpenGLBackend*>(m_backend);

        for (int i = 0; i < 16; i++) {
            LightBuffer& light = m_lightBuffers.emplace_back();

            light.lightProj = createUniform<ShadowPassUniforms>();
            light.lightProj->setValue(ShadowPassUniforms{Mat4::identity()});

            openGLBackend->getUniformManager().buildUniforms({light.lightProj.get()});

            glGenFramebuffers(1, &light.shadowFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, light.shadowFBO);

            glGenTextures(1, &light.shadowTex);
            glBindTexture(GL_TEXTURE_2D, light.shadowTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
                         m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

            // Filtering (use GL_NEAREST for hard shadows, or GL_LINEAR for hardware PCF)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            // Clamp to border with max depth—samples outside the map are "lit"
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light.shadowTex, 0);

            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
                printf("Shadow FBO incomplete: 0x%x\n", status);
            }

            // No color buffer
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
    }

    void ShadowPass::destroyShadowFBO()
    {
        for (auto& light : m_lightBuffers) {
            glDeleteTextures(1, &light.shadowTex);
            glDeleteFramebuffers(1, &light.shadowFBO);
        }
        m_lightBuffers.clear();
    }

    void ShadowPass::setup(void* backend)
    {
        m_backend = backend;
        auto* openGLBackend = static_cast<brew::OpenGLBackend*>(backend);

        m_shaderAsset = MaterialSystem::instance().getShadowShader();
        m_shaderProgram = openGLBackend->getShaderManager().compileShaderFromAsset(m_shaderAsset);
        m_shaderData = openGLBackend->getShaderManager().getShaderData(m_shaderProgram);
    }

    void ShadowPass::resize(int width, int height)
    {
        if (width == 0 || height == 0)
            return;
        if (width == m_width && height == m_height)
            return;

        m_width = width;
        m_height = height;

        destroyShadowFBO();
        createShadowFBO();
    }

    void ShadowPass::execute(void* backend, const std::vector<brew::SortedDrawCommand>& draws)
    {
    }

    void ShadowPass::updateLightingUniforms(void* backend, LightBuffer& lightBuffer,
        const brew::LightInfo& lightInfo)
    {
        auto* openGLBackend = static_cast<brew::OpenGLBackend*>(backend);

        Vec3 lightDir = normalize(Vec3(
            lightInfo.light.directionAndRange.x,
            lightInfo.light.directionAndRange.y,
            lightInfo.light.directionAndRange.z
        ));

        Vec3 lightPos = Vec3(
            lightInfo.light.positionAndType.x,
            lightInfo.light.positionAndType.y,
            lightInfo.light.positionAndType.z
        );

        float lightType = lightInfo.light.positionAndType.w;

        Mat4 lightView;
        Mat4 lightProj;

        if (lightType == 0.0f) {  // Directional
            // For directional lights, position is arbitrary—just need direction
            // Center on origin (or pass in camera target for better quality)
            Vec3 sceneCenter = Vec3(0.0f, 0.0f, 0.0f);
            float shadowDistance = 200.0f;

            Vec3 shadowCamPos = sceneCenter - lightDir * shadowDistance;
            lightView = lookAt(shadowCamPos, sceneCenter, Vec3(0, 1, 0));

            float orthoSize = 100.0f;
            lightProj = ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 1.0f, shadowDistance * 2.0f);

        } else if (lightType == 2.0f) {  // Spot
            // Spot lights have a real position and direction
            Vec3 target = lightPos + lightDir;
            lightView = lookAt(lightPos, target, Vec3(0, 1, 0));

            float range = lightInfo.light.directionAndRange.w;
            float outerCone = lightInfo.light.spotParams.y;
            lightProj = perspective(outerCone * 2.0f, 1.0f, 0.1f, range);
        }

        Mat4 lightViewProj = lightProj * lightView;

        //lightBuffer.lightProj->setData(&lightViewProj, sizeof(Mat4));
        lightBuffer.lightProj->setValue(ShadowPassUniforms{lightViewProj});

        openGLBackend->getUniformManager().updateUniform(*lightBuffer.lightProj);

        // Bind it
        auto* ub = openGLBackend->getUniformManager().getUniformBufferData(*lightBuffer.lightProj);
        if (ub) {
            glBindBufferBase(GL_UNIFORM_BUFFER, 2, ub->ubo);
        }
    }

    void ShadowPass::execute(void* backend, const std::vector<brew::LightInfo>& lights,
        const std::vector<brew::SortedDrawCommand>& draws)
    {
        if (m_shaderProgram == 0)
            return;

        auto* openGLBackend = static_cast<brew::OpenGLBackend*>(backend);

        glUseProgram(m_shaderProgram);

        auto* shadowShaderAsset = AssetManager::instance().get<ShaderAsset>(m_shaderAsset);

        for (size_t i = 0; i < lights.size(); ++i) {
            auto& light = lights[i];
            auto& lightBuffer = m_lightBuffers.at(i);

            if (!light.castsShadow) continue;

            // bind this light's shadow FBO
            glBindFramebuffer(GL_FRAMEBUFFER, lightBuffer.shadowFBO);
            glViewport(0, 0, m_width, m_height);
            glClear(GL_DEPTH_BUFFER_BIT);

            updateLightingUniforms(m_backend, lightBuffer, light);

            for (const auto& draw : draws) {
                auto& cmd = draw.command;
            //    if (!draw.castsShadows) continue;

                brew::OpenGLMeshData* meshData = openGLBackend->getMeshManager().getOrCreateMeshData(cmd.mesh);
                if (!meshData)
                    continue;

                if (!meshData->isUploaded) {
                    continue;
                }

                GLuint vao = openGLBackend->getMeshManager().getOrCreateVAO(meshData, m_shaderProgram, shadowShaderAsset);
                if (vao == 0)
                    continue;

                glBindVertexArray(vao);

                openGLBackend->updatePerDrawUniforms(cmd, m_shaderData);

                if (meshData->indexCount > 0) {
                    glDrawElements(GL_TRIANGLES,
                                   cmd.indexCount,
                                   GL_UNSIGNED_INT,
                                   (void*)(cmd.indexOffset * sizeof(uint32_t)));
                } else {
                    glDrawArrays(GL_TRIANGLES, 0, meshData->vertexCount);
                }
            }
        }

        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}