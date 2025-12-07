#pragma once
#include "OpenGLShader.h"
#include <ChaiEngine/UniformBuffer.h>
#include <Graphics/RenderPass.h>

#include <glad/gl.h>

namespace  chai::brew
{
    struct ShadowPassUniforms
    {
        Mat4 projection = Mat4::identity();
    };

    struct LightBuffer
    {
        GLuint shadowFBO = 0;
        GLuint shadowTex = 0;
        std::shared_ptr<UniformBuffer<ShadowPassUniforms>> lightProj = nullptr;
    };

    class ShadowPass : public brew::RenderPass
    {
    public:
        ShadowPass();
        ~ShadowPass() override;

        void setup(void* backend) override;
        void resize(int width, int height) override;
        void execute(void* backend, const std::vector<brew::SortedDrawCommand>& draws) override;
        void execute(void* backend, const std::vector<brew::LightInfo>& lights,
            const std::vector<brew::SortedDrawCommand>& draws);

        std::vector<LightBuffer>& getLightBuffers() { return m_lightBuffers; }
        std::vector<LightInfo>& getLights() { return m_lights; }

    private:
        static const int SHADOW_MAP_SIZE = 4096;
        /*int m_width = 0;
        int m_height = 0;*/
        void* m_backend = nullptr;
        void createShadowFBO();
        void destroyShadowFBO();
        void updateLightingUniforms(void* backend, LightBuffer& lightBuffer,
            const LightInfo& lightInfo);

        AssetHandle m_shaderAsset;
        GLuint m_shaderProgram;
        OpenGLShaderData* m_shaderData;
        std::vector<LightBuffer> m_lightBuffers;
        std::vector<LightInfo> m_lights;
    };
}
