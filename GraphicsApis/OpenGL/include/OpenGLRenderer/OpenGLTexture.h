#pragma once
#include <Core/TextureBackend.h>
#include <glad/glad.h>
#include <memory>

namespace chai::brew
{
    class OpenGLTextureBackend : public ITextureBackend {
    public:
        OpenGLTextureBackend();
        OpenGLTextureBackend(const uint8_t* data, uint32_t width, uint32_t height);
        ~OpenGLTextureBackend();

        void Bind(uint32_t slot) override;
        void* GetNativeHandle() override;

    private:
        GLuint m_textureID = 0;
        uint32_t m_width = 0;
        uint32_t m_height = 0;
    };
}