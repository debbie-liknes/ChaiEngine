#pragma once
#include <Resource/ResourceLoader.h>
#include <Core/TextureBackend.h>

namespace chai
{
    class Texture2D : public IResource {
    public:
        Texture2D();

        bool create(uint32_t width, uint32_t height, const uint8_t* data = nullptr);

        const std::string& GetName() const override { return m_name; }

        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }

        // Backend interface
        std::shared_ptr<ITextureBackend> GetBackendHandle() const;

    private:
        std::string m_name;
        uint32_t m_width = 0;
        uint32_t m_height = 0;

        std::shared_ptr<ITextureBackend> m_backend;
    };
}