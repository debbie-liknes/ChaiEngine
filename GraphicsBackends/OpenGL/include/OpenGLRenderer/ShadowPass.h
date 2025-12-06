#pragma once
#include "OpenGLShader.h"
#include <ChaiEngine/UniformBuffer.h>
#include <Graphics/RenderPass.h>

#include <glad/gl.h>

namespace  chai
{
    struct ShadowPassUniforms
    {
        Mat4 projection;
    };

    struct LightBuffer
    {
        GLuint shadowFBO;
        GLuint shadowTex;
        std::shared_ptr<UniformBuffer<ShadowPassUniforms>> lightProj;
    };

    class GBufferPass;
    class ShadowPass : public brew::RenderPass
    {
    public:
        ShadowPass() = default;
        ~ShadowPass() override;

        void setup(void* backend) override;
        void resize(int width, int height) override;
        void execute(void* backend, const std::vector<brew::SortedDrawCommand>& draws) override;
        void execute(void* backend, const std::vector<brew::LightInfo>& lights,
            const std::vector<brew::SortedDrawCommand>& draws);

    private:
        int m_width, m_height;
        void* m_backend;
        void createShadowFBO();
        void destroyShadowFBO();
        void updateLightingUniforms(void* backend, LightBuffer& lightBuffer,
            const brew::LightInfo& lightInfo);

        AssetHandle m_shaderAsset;
        GLuint m_shaderProgram;
        brew::OpenGLShaderData* m_shaderData;
        std::vector<LightBuffer> m_lightBuffers;
    };
}
