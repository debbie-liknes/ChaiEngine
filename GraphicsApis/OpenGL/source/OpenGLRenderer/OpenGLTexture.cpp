#include <OpenGLRenderer/OpenGLTexture.h>

namespace chai::brew
{
    OpenGLTextureBackend::OpenGLTextureBackend()
    {

    }

    OpenGLTextureBackend::OpenGLTextureBackend(const uint8_t* data, uint32_t width, uint32_t height)
        : m_width(width), m_height(height)
    {
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA8,
            width, height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, data
        );

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    OpenGLTextureBackend::~OpenGLTextureBackend()
    {

    }

    void OpenGLTextureBackend::Bind(uint32_t slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
    }

    void* OpenGLTextureBackend::GetNativeHandle()
    {
        return nullptr;
    }
}