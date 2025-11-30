#pragma once
#include <Graphics/RenderPass.h>
#include <Graphics/RenderKey.h>
#include <glad/gl.h>

namespace chai::brew
{
    class GBufferPass;
    class LightingPass : public RenderPass
    {
    public:
        LightingPass() = default;
        ~LightingPass() override;
        LightingPass(GBufferPass* gbufferPass);

        void setup(void* backend) override;

        void execute(void* backend, const std::vector<SortedDrawCommand>& draws) override;

    private:
        //void createFullscreenQuad();
        //void createLightingShader();

        GBufferPass* m_gbufferPass;
        GLuint m_lightingShader = 0;
        GLuint m_quadVAO = 0;
        GLuint m_quadVBO = 0;
    };
}