#include <Types/Texture2D.h>

namespace chai
{
    Texture2D::Texture2D()
    {

    }

    bool Texture2D::Create(uint32_t width, uint32_t height, const uint8_t* data)
    {
        m_width = width;
        m_height = height;

        //m_backend = RendererAPI::Instance().CreateTexture2D(data, width, height);

        return m_backend != nullptr;
    }

    std::shared_ptr<ITextureBackend> Texture2D::GetBackendHandle() const
    {
        return m_backend;
    }
}