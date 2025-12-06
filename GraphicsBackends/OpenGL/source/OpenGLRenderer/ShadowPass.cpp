#include "ChaiEngine/MaterialSystem.h"
#include "OpenGLRenderer/OpenGLRenderer.h"

#include <OpenGLRenderer/ShadowPass.h>

namespace chai
{

    ShadowPass::~ShadowPass()
    {}

    void ShadowPass::setup(void* backend)
    {
        // Setup (done once)
        glGenFramebuffers(1, &m_shadowMap);
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMap);

        glGenTextures(1, &m_shadowMap);
        glBindTexture(GL_TEXTURE_2D, m_shadowMap);
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

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap, 0);

        // No color buffer
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        auto* openGLBackend = static_cast<brew::OpenGLBackend*>(backend);

        /*m_shaderAsset = MaterialSystem::instance().getGBufferShader();
        m_shaderProgram = openGLBackend->getShaderManager().compileShaderFromAsset(m_shaderAsset);
        m_shaderData = openGLBackend->getShaderManager().getShaderData(m_shaderProgram);*/
    }

    void ShadowPass::resize(int width, int height)
    {
        m_width = width;
        m_height = height;
    }

    void ShadowPass::execute(void* backend, const std::vector<brew::SortedDrawCommand>& draws)
    {
        /*if (m_shaderProgram == 0)
            return;*/

        auto* openGLBackend = static_cast<brew::OpenGLBackend*>(backend);

        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMap);
        glViewport(0, 0, m_width, m_height);
        glClear(GL_DEPTH_BUFFER_BIT);

        //glUseProgram(m_shaderProgram);

    }
}