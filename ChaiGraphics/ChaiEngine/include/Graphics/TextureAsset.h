#pragma once
#include <Asset/AssetLoader.h>
#include <Asset/AssetManager.h>
#include <Resource/Resource.h>
#include <Resource/ResourceManager.h>

namespace chai
{
    class TextureAsset : public IAsset
    {
    public:
        explicit TextureAsset(const int width, const int height, const int channels, uint8_t* pixels) :
            m_width(width), m_height(height), m_channels(channels), m_pixels(pixels)
        {}

        bool isValid() const override { return true; }
        const std::string& getAssetId() const override { return m_assetId; }

        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }
        int getChannels() const { return m_channels; }
        uint8_t* getPixels() const { return m_pixels; }


    private:
        int m_width = 0;
        int m_height = 0;
        int m_channels = 0;
        uint8_t* m_pixels = nullptr;
    };

    class TextureResource : public Resource
    {
    public:
        explicit TextureResource(AssetHandle asset) : m_assetHandle(asset)
        {
            init();
        }

        int getWidth() { return m_width; }
        int getHeight() { return m_height; }
        int getChannels() { return m_channels; }
        uint8_t* getPixels() { return m_pixels.data(); }

    private:
        void init()
        {
            if (m_assetHandle.isValid()) {
                auto textureAsset = AssetManager::instance().get<TextureAsset>(m_assetHandle);
                m_width = textureAsset->getWidth();
                m_height = textureAsset->getHeight();
                m_channels = textureAsset->getChannels();
                m_pixels.resize(m_width * m_height * m_channels);
                memcpy(m_pixels.data(), textureAsset->getPixels(), m_width * m_height * m_channels);
            }
        }

        AssetHandle m_assetHandle;

        int m_width = 0;
        int m_height = 0;
        int m_channels = 0;
        std::vector<uint8_t> m_pixels;
    };

    static std::optional<ResourceHandle> loadTexture(const std::string& path)
    {
        auto textureAsset = AssetManager::instance().load<TextureAsset>(path);
        if (textureAsset.has_value()) {
            auto textureResource = std::make_unique<TextureResource>(textureAsset.value());
            return ResourceManager::instance().add<TextureResource>(std::move(textureResource));
        }
        return std::nullopt;
    }
}