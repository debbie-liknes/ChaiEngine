#pragma once
#include "Graphics/RenderPass.h"
#include <glad/gl.h>

namespace  chai
{
    class GBufferPass;
    class ShadowPass : public brew::RenderPass
    {
    public:
        ShadowPass() = default;
        ~ShadowPass() override;

        void setup(void* backend) override;
        void resize(int width, int height) override;
        void execute(void* backend, const std::vector<brew::SortedDrawCommand>& draws) override;

    private:
        int m_width, m_height;
        GLuint m_shadowMap;
    };
}
