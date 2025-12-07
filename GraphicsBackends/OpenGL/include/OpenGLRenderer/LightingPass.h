#pragma once
#include <Graphics/RenderPass.h>
#include <Graphics/RenderKey.h>
#include <glad/gl.h>

namespace chai::brew
{
    class GBufferPass;
    class ShadowPass;
    class LightingPass : public RenderPass
    {
    public:
        LightingPass() = default;
        ~LightingPass() override;
        LightingPass(GBufferPass* gbufferPass, ShadowPass* shadowPass);

        void setup(void* backend) override;

        void execute(void* backend, const std::vector<brew::LightInfo>& lights);

    private:

        GBufferPass* m_gbufferPass = nullptr;
        ShadowPass* m_shadowPass = nullptr;
        GLuint m_lightingShader = 0;
        GLuint m_quadVAO = 0;
        GLuint m_quadVBO = 0;
    };
}