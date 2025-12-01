#pragma once
#include <Graphics/RenderPass.h>
#include <OpenGLRenderer/GBuffer.h>
#include <OpenGLRenderer/OpenGLShader.h>
#include <glad/gl.h>

namespace chai::brew
{
    class GBufferPass : public RenderPass
    {
    public:
        GBufferPass();

        void setup(void* backend) override;

        void execute(void* backend, const std::vector<SortedDrawCommand>& draws) override;

        void resize(int width, int height) override;

        GLuint getPositionTex() const { return m_gbuffer.positionTex; }
        GLuint getNormalTex() const { return m_gbuffer.normalTex; }
        GLuint getAlbedoTex() const { return m_gbuffer.albedoTex; }
        GLuint getMaterialTex() const { return m_gbuffer.materialTex; }
        GLuint getDepthTex() const { return m_gbuffer.depthTex; }
        GLuint getFramebuffer() const { return m_gbuffer.framebuffer; }

    private:
        void destroyGBuffer();

        GBuffer m_gbuffer;

        AssetHandle m_shaderAsset;
        GLuint m_shaderProgram = 0;
        OpenGLShaderData* m_shaderData = nullptr;

        int m_width = 0;
        int m_height = 0;
    };
}