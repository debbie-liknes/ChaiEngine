#pragma once
#include "Graphics/RenderPass.h"

namespace  chai
{
    class GBufferPass;
    class ShadowPass : public brew::RenderPass
    {
    public:
        ShadowPass() = default;
        ~ShadowPass() override;
        ShadowPass(GBufferPass* gbufferPass);

        void execute(void* backend, const std::vector<brew::SortedDrawCommand>& draws) override;

    private:
        GBufferPass* m_gbufferPass;
        int m_width, m_height;
    };
}
