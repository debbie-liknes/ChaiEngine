// SkyboxPass.h
#pragma once
#include <Graphics/RenderPass.h>
#include <OpenGLRenderer/GBuffPass.h>
#include <glad/gl.h>

namespace chai::brew
{
    class SkyboxPass : public RenderPass
    {
    public:
        SkyboxPass();
        ~SkyboxPass();
        SkyboxPass(GBufferPass* gbufferPass);

        void setup(void* backend) override;
        void execute(void* backend, const std::vector<SortedDrawCommand>& draws) override;
        void resize(int width, int height) override;

    private:
        void createCube();

        GBufferPass* m_gbufferPass;

        GLuint m_shaderProgram = 0;
        GLuint m_cubeVAO = 0;
        GLuint m_cubeVBO = 0;

        int m_width = 0;
        int m_height = 0;
    };
}