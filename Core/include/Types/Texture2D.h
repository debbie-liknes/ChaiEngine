#pragma once
#include <Resource/ResourceLoader.h>
#include <Core/TextureBackend.h>

namespace chai
{
    class Texture2D : public IResource {
    public:
        Texture2D(){}
        Texture2D(std::shared_ptr<ITextureBackend> backend, uint32_t width, uint32_t height);

        const std::string GetName() const { return m_name; }
        std::shared_ptr<ITextureBackend> GetBackendHandle() const;

    private:
        std::string m_name;
        uint32_t m_width = 0;
        uint32_t m_height = 0;
        std::shared_ptr<ITextureBackend> m_backend;
    };
}