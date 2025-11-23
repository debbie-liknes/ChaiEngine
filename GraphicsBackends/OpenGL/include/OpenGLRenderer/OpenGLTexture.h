#pragma once
#include <glad/gl.h>
#include <Types/CMap.h>

namespace chai::brew
{
struct OpenGLTextureData
{
    GLuint textureID = 0;
    bool isUploaded = false;
    GLenum format = GL_RGBA;
    GLenum internalFormat = GL_RGBA8;
    int width = 0;
    int height = 0;
};

class OpenGLTextureManager
{
public:
    OpenGLTextureData* getOrCreateTextureData(Handle textureHandle)
    {
        auto it = m_textureCache.find(textureHandle.index);
        if (it == m_textureCache.end()) {
            auto glTextureData = std::make_unique<OpenGLTextureData>();
            auto* ptr = glTextureData.get();
            m_textureCache[textureHandle.index] = std::move(glTextureData);
            return ptr;
        }
        return it->second.get();
    }

private:
    CMap<size_t, std::unique_ptr<OpenGLTextureData>> m_textureCache;
};
}