#include <OpenGLRenderer/ForwardPass.h>
#include <OpenGLRenderer/GBuffPass.h>


namespace chai::brew
{
    ForwardPass::~ForwardPass()
    {}

    ForwardPass::ForwardPass(GBufferPass* gbufferPass) : m_gbufferPass(gbufferPass)
    {
        m_desc.type = RenderPassDesc::Type::Forward;
        m_desc.clearColor = false;
        m_desc.clearDepth = false;
    }

    void ForwardPass::execute(void* backend, const std::vector<SortedDrawCommand>& draws)
    {
        return;
        // Copy depth from G-Buffer to default framebuffer
        // glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbufferPass->getFramebuffer());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(
            0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE); // Don't write depth for transparent
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw transparent objects with forward shading
        for (const auto& draw : draws) {
            // ...
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
} // namespace chai::brew