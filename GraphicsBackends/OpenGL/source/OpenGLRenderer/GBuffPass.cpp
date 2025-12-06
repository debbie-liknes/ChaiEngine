#include <OpenGLRenderer/GBuffPass.h>
#include <OpenGLRenderer/GBuffer.h>
#include <OpenGLRenderer/OpenGLRenderer.h>
#include <ChaiEngine/MaterialSystem.h>

namespace chai::brew
{
    GBufferPass::GBufferPass()
    {
        m_desc.type = RenderPassDesc::Type::GBuffer;
        m_desc.clearColor = true;
        m_desc.clearDepth = true;
    }

    void GBufferPass::setup(void* backend)
    {
        auto* openGLBackend = static_cast<OpenGLBackend*>(backend);

        // Get the G-Buffer shader from MaterialSystem
        m_shaderAsset = MaterialSystem::instance().getGBufferShader();
        m_shaderProgram = openGLBackend->getShaderManager().compileShaderFromAsset(m_shaderAsset);
        m_shaderData = openGLBackend->getShaderManager().getShaderData(m_shaderProgram);
    }

    void GBufferPass::execute(void* backend, const std::vector<SortedDrawCommand>& draws)
    {
        if (m_shaderProgram == 0)
            return;

        auto* openGLBackend = static_cast<OpenGLBackend*>(backend);

        glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer.framebuffer);
        glViewport(0, 0, m_width, m_height);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_BLEND);
        DepthStencilState depthState;
        depthState.depthMaskEnable = true;
        depthState.depthTestEnable = true;

        openGLBackend->getCurrentState().updateDepthState(depthState);

        glUseProgram(m_shaderProgram);

        openGLBackend->updatePerFrameUniforms();

        auto* gbufferShaderAsset = AssetManager::instance().get<ShaderAsset>(m_shaderAsset);

        for (const auto& draw : draws) {
            const auto& cmd = draw.command;

            // Get mesh data
            OpenGLMeshData* meshData =
                openGLBackend->getMeshManager().getOrCreateMeshData(cmd.mesh);
            if (!meshData)
                continue;

            // Schedule mesh upload if not ready
            if (!meshData->isUploaded) {
                if (!openGLBackend->getUploadQueue().isQueued(cmd.mesh)) {
                    openGLBackend->getUploadQueue().requestUpload(
                        cmd.mesh,
                        (void*)meshData,
                        UploadType::MESH,
                        (void*)&openGLBackend->getMeshManager());
                }
                continue; // Skip this frame, will render next frame
            }

            // Get material data
            OpenGLMaterialData* matData =
                openGLBackend->getMaterialManager().getOrCreateMaterialData(cmd.material);
            if (!matData)
                continue;

            openGLBackend->getMaterialManager().compileMaterial(cmd.material, matData);

            // Check textures
            bool waitingOnTextures = false;
            for (const auto& [name, texHandle] : matData->textures) {
                if (!texHandle.isValid())
                    continue;

                OpenGLTextureData* texData =
                    openGLBackend->getTextureManager().getOrCreateTextureData(texHandle);
                if (!texData)
                    continue;

                if (!texData->isUploaded) {
                    if (!openGLBackend->getUploadQueue().isQueued(texHandle)) {
                        openGLBackend->getUploadQueue().requestUpload(
                            texHandle,
                            (void*)texData,
                            UploadType::TEXTURE,
                            (void*)&openGLBackend->getTextureManager());
                    }
                    waitingOnTextures = true;
                }
            }
            if (waitingOnTextures)
                continue;

            // Get VAO
            GLuint vao = openGLBackend->getMeshManager().getOrCreateVAO(
                meshData, m_shaderProgram, gbufferShaderAsset);
            if (vao == 0)
                continue;

            glBindVertexArray(vao);

            openGLBackend->updatePerDrawUniforms(cmd, m_shaderData);
            openGLBackend->updateLightUniforms(m_shaderData);
            openGLBackend->applyMaterialState(matData, m_shaderData);

            openGLBackend->getCurrentState().updateDepthState(cmd.pipelineState.depthStencilState);
            openGLBackend->getCurrentState().updateRasterizerState(cmd.pipelineState.rasterState);

            if (meshData->indexCount > 0) {
                glDrawElements(GL_TRIANGLES,
                               cmd.indexCount,
                               GL_UNSIGNED_INT,
                               (void*)(cmd.indexOffset * sizeof(uint32_t)));
            } else {
                glDrawArrays(GL_TRIANGLES, 0, meshData->vertexCount);
            }
        }

        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GBufferPass::destroyGBuffer()
    {
        if (m_gbuffer.framebuffer) {
            glDeleteFramebuffers(1, &m_gbuffer.framebuffer);
            m_gbuffer.framebuffer = 0;
        }
        if (m_gbuffer.positionTex) {
            glDeleteTextures(1, &m_gbuffer.positionTex);
            m_gbuffer.positionTex = 0;
        }
        if (m_gbuffer.normalTex) {
            glDeleteTextures(1, &m_gbuffer.normalTex);
            m_gbuffer.normalTex = 0;
        }
        if (m_gbuffer.albedoTex) {
            glDeleteTextures(1, &m_gbuffer.albedoTex);
            m_gbuffer.albedoTex = 0;
        }
        if (m_gbuffer.materialTex) {
            glDeleteTextures(1, &m_gbuffer.materialTex);
            m_gbuffer.materialTex = 0;
        }
        if (m_gbuffer.depthTex) {
            glDeleteTextures(1, &m_gbuffer.depthTex);
            m_gbuffer.depthTex = 0;
        }
    }


    void GBufferPass::resize(int width, int height)
    {
        if (width == 0 || height == 0)
            return;
        if (width == m_width && height == m_height)
            return;

        m_width = width;
        m_height = height;
        destroyGBuffer();
        createGBuffer(m_gbuffer, width, height);
    }
}