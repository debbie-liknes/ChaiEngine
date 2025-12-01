#include "ChaiEngine/MaterialSystem.h"
#include "OpenGLRenderer/OpenGLRenderer.h"

#include <OpenGLRenderer/SkyboxPass.h>

namespace chai::brew
{
    SkyboxPass::SkyboxPass()
    {
    }

    SkyboxPass::SkyboxPass(GBufferPass* gbufferPass)
    {
        m_gbufferPass = gbufferPass;
    }



    SkyboxPass::~SkyboxPass()
    {
        if (m_cubeVAO) glDeleteVertexArrays(1, &m_cubeVAO);
        if (m_cubeVBO) glDeleteBuffers(1, &m_cubeVBO);
    }

    void SkyboxPass::setup(void* backend)
    {
        auto* openGLBackend = static_cast<OpenGLBackend*>(backend);

        m_desc.type = RenderPassDesc::Type::Skybox;
        m_desc.clearColor = false;  // Don't clear - we render after lighting
        m_desc.clearDepth = false;

        AssetHandle skyboxShaderAsset = MaterialSystem::instance().getSkyboxShader();
        m_shaderProgram =
            openGLBackend->getShaderManager().compileShaderFromAsset(skyboxShaderAsset);

        createCube();
    }

    void SkyboxPass::createCube()
    {
        float vertices[] = {
            // Back face
            -1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            // Front face
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
            // Left face
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            // Right face
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            // Bottom face
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            // Top face
            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
        };

        glGenVertexArrays(1, &m_cubeVAO);
        glGenBuffers(1, &m_cubeVBO);

        glBindVertexArray(m_cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);
    }

    void SkyboxPass::execute(void* backend, const std::vector<SortedDrawCommand>& draws)
    {
        if (m_shaderProgram == 0 || m_cubeVAO == 0) return;
        if (draws.empty()) return;

        auto* openGLBackend = static_cast<OpenGLBackend*>(backend);
        const auto& cmd = draws[0].command;

        // Copy depth from G-Buffer to default framebuffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbufferPass->getFramebuffer());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(
            0, 0, m_width, m_height,
            0, 0, m_width, m_height,
            GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Depth test with LEQUAL so skybox passes at depth 1.0
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);

        glUseProgram(m_shaderProgram);
        openGLBackend->updatePerFrameUniforms();

        // Bind skybox texture
        if (cmd.skybox.isValid()) {
            OpenGLTextureData* texData =
                openGLBackend->getTextureManager().getOrCreateTextureData(cmd.skybox);

            if (texData && !texData->isUploaded && !openGLBackend->getUploadQueue().isQueued(cmd.skybox)) {
                openGLBackend->getUploadQueue().requestUpload(
                    cmd.skybox,
                    (void*)texData,
                    UploadType::TEXTURE,
                    (void*)&openGLBackend->getTextureManager());
                return;
            }

            if (texData && texData->isUploaded) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texData->texture);
                glUniform1i(glGetUniformLocation(m_shaderProgram, "u_Skybox"), 0);
            }
        }

        glBindVertexArray(m_cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // Restore state
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
    }

    /*void SkyboxPass::execute(void* backend, const std::vector<SortedDrawCommand>& draws)
    {
        if (m_shaderProgram == 0 || m_cubeVAO == 0) return;
        if (draws.empty()) return;

        auto* openGLBackend = static_cast<OpenGLBackend*>(backend);
        const auto& cmd = draws[0].command;


        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbufferPass->getFramebuffer());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(
            0, 0, m_width, m_height,
            0, 0, m_width, m_height,
            GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        openGLBackend->getCurrentState().updateRasterizerState(cmd.pipelineState.rasterState);
        openGLBackend->getCurrentState().updateDepthState(cmd.pipelineState.depthStencilState);
        //glEnable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LEQUAL);
        //glDepthMask(GL_FALSE);
        //glDisable(GL_CULL_FACE);

        glUseProgram(m_shaderProgram);

        // Update view/projection uniforms
        openGLBackend->updatePerFrameUniforms();

        // Bind the skybox cubemap texture from the draw command
        if (cmd.skybox.isValid()) {
            OpenGLTextureData* texData =
                openGLBackend->getTextureManager().getOrCreateTextureData(cmd.skybox);

            if (texData && !texData->isUploaded && !openGLBackend->getUploadQueue().isQueued(cmd.skybox)) {
                std::cout << "upload skybox" << std::endl;

                openGLBackend->getUploadQueue().requestUpload(
                    cmd.skybox,
                    (void*)texData,
                    UploadType::TEXTURE,
                    (void*)&openGLBackend->getTextureManager());
                return;
            }

            if (texData && texData->isUploaded) {
                std::cout << "uploaded" << std::endl;
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texData->texture);
                glUniform1i(glGetUniformLocation(m_shaderProgram, "u_Skybox"), 0);
            }
        }

        // Draw the cube
        glBindVertexArray(m_cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // Restore state
        //glDepthFunc(GL_LESS);
        //glDepthMask(GL_TRUE);
        //glEnable(GL_CULL_FACE);
    }*/

    void SkyboxPass::resize(int width, int height)
    {
        m_width = width;
        m_height = height;
    }
}