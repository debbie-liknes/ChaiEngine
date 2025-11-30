// LightingPass.cpp
#include <ChaiEngine/MaterialSystem.h>
#include <OpenGLRenderer/GBuffPass.h>
#include <OpenGLRenderer/LightingPass.h>
#include <OpenGLRenderer/OpenGLRenderer.h>

namespace chai::brew
{
    LightingPass::~LightingPass()
    {
        if (m_quadVAO) {
            glDeleteVertexArrays(1, &m_quadVAO);
            m_quadVAO = 0;
        }
        if (m_quadVBO) {
            glDeleteBuffers(1, &m_quadVBO);
            m_quadVBO = 0;
        }
    }

    LightingPass::LightingPass(GBufferPass* gbufferPass) : m_gbufferPass(gbufferPass)
    {
        m_desc.type = RenderPassDesc::Type::Lighting;
        m_desc.clearColor = true;
        m_desc.clearDepth = false;
    }

    void LightingPass::setup(void* backend)
    {
        auto* openGLBackend = static_cast<OpenGLBackend*>(backend);

        // Compile lighting shader
        AssetHandle lightingShaderAsset = MaterialSystem::instance().getLightingShader();
        m_lightingShader =
            openGLBackend->getShaderManager().compileShaderFromAsset(lightingShaderAsset);

        // Create fullscreen quad
        float quadVertices[] = {
            // positions   // texcoords
            -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};

        glGenVertexArrays(1, &m_quadVAO);
        glGenBuffers(1, &m_quadVBO);

        glBindVertexArray(m_quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
    }

    void LightingPass::execute(void* backend, const std::vector<SortedDrawCommand>& draws)
    {
        if (m_lightingShader == 0 || m_quadVAO == 0)
            return;

        auto* openGLBackend = static_cast<OpenGLBackend*>(backend);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        glUseProgram(m_lightingShader);

        // Bind G-Buffer textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_gbufferPass->getPositionTex());
        glUniform1i(glGetUniformLocation(m_lightingShader, "gPosition"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_gbufferPass->getNormalTex());
        glUniform1i(glGetUniformLocation(m_lightingShader, "gNormal"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_gbufferPass->getAlbedoTex());
        glUniform1i(glGetUniformLocation(m_lightingShader, "gAlbedo"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_gbufferPass->getMaterialTex());
        glUniform1i(glGetUniformLocation(m_lightingShader, "gMaterial"), 3);

        // Update lighting uniforms
        auto* shaderData = openGLBackend->getShaderManager().getShaderData(m_lightingShader);
        openGLBackend->updatePerFrameUniforms();
        openGLBackend->updateLightUniforms(shaderData);

        // Draw fullscreen quad
        glBindVertexArray(m_quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
} // namespace chai::brew