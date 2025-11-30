#pragma once
#include <Graphics/RenderPass.h>
#include <Graphics/RenderKey.h>

namespace chai::brew
{
    class GBufferPass;
    class ForwardPass : public RenderPass
    {
    public:
        ForwardPass() = default;
        ~ForwardPass() override;
        ForwardPass(GBufferPass* gbufferPass);

        void execute(void* backend, const std::vector<SortedDrawCommand>& draws) override;

    private:
        GBufferPass* m_gbufferPass;
        int m_width, m_height;
    };
}