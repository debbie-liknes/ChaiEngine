#include <Types/Texture2D.h>

namespace chai
{
    Texture2D::Texture2D(std::shared_ptr<ITextureBackend> backend, uint32_t width, uint32_t height)
        : m_backend(std::move(backend)), m_width(width), m_height(height) 
    {
    }

    std::shared_ptr<ITextureBackend> Texture2D::GetBackendHandle() const
    {
        return m_backend;
    }
}