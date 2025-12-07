#pragma once
#include "OpenGLShader.h"
#include <ChaiEngine/UniformBuffer.h>
#include <Graphics/RenderPass.h>

#include <glad/gl.h>

namespace  chai::brew
{
    static const int NUM_CASCADES = 4;
    static const int SHADOW_MAP_SIZE = 4096;

    struct ShadowPassUniforms
    {
        Mat4 projection = Mat4::identity();
    };

    struct CascadeData
    {
        GLuint shadowFBO = 0;
        GLuint shadowTex = 0;
        Mat4 lightViewProj = Mat4::identity();
        float splitDepth = 0.0f;
    };

    struct LightShadowData
    {
        CascadeData cascades[NUM_CASCADES];
        std::shared_ptr<UniformBuffer<ShadowPassUniforms>> lightProj = nullptr;
    };

    /*struct LightBuffer
    {
        GLuint shadowFBO = 0;
        GLuint shadowTex = 0;
        std::shared_ptr<UniformBuffer<ShadowPassUniforms>> lightProj = nullptr;
    };*/

    class ShadowPass : public brew::RenderPass
    {
    public:
        ShadowPass();
        ~ShadowPass() override;

        void setup(void* backend) override;
        void resize(int width, int height) override;
        void execute(void* backend,
             const std::vector<LightInfo>& lights,
             const std::vector<SortedDrawCommand>& draws,
             const Mat4& cameraView,
             const Mat4& cameraProj,
             float nearPlane,
             float farPlane);

        std::vector<LightShadowData>& getLightShadowData() { return m_lightShadowData; }
        std::vector<LightInfo>& getLights() { return m_lights; }
        const float* getCascadeSplits() const { return m_cascadeSplits; }

    private:
        void createShadowFBO();
        void destroyShadowFBO();
        void calculateCascadeSplits(float nearPlane, float farPlane);
        std::vector<Vec3> getFrustumCornersWorldSpace(const Mat4& viewProj);
        Mat4 calculateLightViewProjForCascade(const Vec3& lightDir,
                                               const Mat4& cameraView,
                                               const Mat4& cameraProj,
                                               float nearSplit,
                                               float farSplit);

        int m_width = 0;
        int m_height = 0;
        void* m_backend = nullptr;

        AssetHandle m_shaderAsset;
        GLuint m_shaderProgram;
        OpenGLShaderData* m_shaderData;

        std::vector<LightShadowData> m_lightShadowData;
        std::vector<LightInfo> m_lights;
        float m_cascadeSplits[NUM_CASCADES + 1];
    };
}
