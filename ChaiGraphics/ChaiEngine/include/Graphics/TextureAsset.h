#pragma once
#include <Asset/AssetLoader.h>
#include <Asset/AssetManager.h>
#include <Resource/Resource.h>
#include <Resource/ResourceManager.h>

#include <filesystem>
#include <utility>
#include <vector>

namespace chai
{
    enum class ColorSpace
    {
        Linear,
        SRGB
    };

    enum class TextureType
    {
        Tex2D,
        TexCube
    };

    struct TextureFace
    {
        int width  = 0;
        int height = 0;
        int channels = 0;
        std::vector<uint8_t> pixels;
    };

    enum class TextureWrapMode
    {
        Clamp,
        Mirror
    };

    class TextureAsset : public IAsset
    {
    public:
        explicit TextureAsset(std::vector<TextureFace> faces, ColorSpace space = ColorSpace::SRGB,
            TextureType type = TextureType::Tex2D, TextureWrapMode mode = TextureWrapMode::Clamp) :
            m_space(space), m_type(type), m_faces(std::move(faces)), m_wrapMode(mode)
        {}

        bool isValid() const override { return !m_faces.empty(); }
        const std::string& getAssetId() const override { return m_assetId; }

        int getFaceCount() const { return m_faces.size(); }
        const TextureFace& getFace(size_t i) const { return m_faces[i]; }
        TextureFace&       getFace(size_t i)       { return m_faces[i]; }

        int getWidth() const { return m_faces.front().width; }
        int getHeight() const { return m_faces.front().height; }
        int getChannels() const { return m_faces.front().channels; }
        const unsigned char* getPixels() const { return m_faces.front().pixels.data(); }
        ColorSpace getColorSpace() const { return m_space; }
        TextureType getType() const { return m_type; }
        TextureWrapMode getWrapMode() const { return m_wrapMode; }


    private:
        std::vector<TextureFace> m_faces;
        ColorSpace m_space = ColorSpace::SRGB;
        TextureType m_type = TextureType::Tex2D;
        TextureWrapMode m_wrapMode = TextureWrapMode::Clamp;
    };

    class TextureResource : public Resource
    {
    public:
        explicit TextureResource(AssetHandle asset) : m_assetHandle(asset)
        {
            init();
        }

        int getFaceCount() const { return m_faces.size(); }
        const TextureFace& getFace(size_t i) const { return m_faces[i]; }
        TextureFace&       getFace(size_t i)       { return m_faces[i]; }

        int getWidth() { return m_faces.front().width; }
        int getWidth() const { return m_faces.front().width; }
        int getHeight() { return m_faces.front().height; }
        int getHeight() const { return m_faces.front().height; }
        int getChannels() { return m_faces.front().channels; }
        int getChannels() const { return m_faces.front().channels; }
        ColorSpace getColorSpace() { return m_space; }
        ColorSpace getColorSpace() const { return m_space; }
        TextureType getType() const { return m_type; }
        TextureType getType() { return m_type; }
        uint8_t* getPixels() { return m_faces.front().pixels.data(); }
        TextureWrapMode getWrapMode() const { return m_wrapMode; }
        TextureWrapMode getWrapMode() { return m_wrapMode; }

    private:
        void init()
        {
            if (m_assetHandle.isValid()) {
                auto textureAsset = AssetManager::instance().get<TextureAsset>(m_assetHandle);

                for (int i = 0; i < textureAsset->getFaceCount(); i++) {
                    auto& face = m_faces.emplace_back();
                    face.width = textureAsset->getWidth();
                    face.height = textureAsset->getHeight();
                    face.channels = textureAsset->getChannels();
                    face.pixels.resize(face.width * face.height * face.channels);
                    face.pixels = textureAsset->getFace(i).pixels;
                }
                m_space = textureAsset->getColorSpace();
                m_type = textureAsset->getType();
                m_wrapMode = textureAsset->getWrapMode();
            }
        }

        AssetHandle m_assetHandle;

        std::vector<TextureFace> m_faces;
        ColorSpace m_space = ColorSpace::SRGB;
        TextureType m_type = TextureType::Tex2D;
        TextureWrapMode m_wrapMode = TextureWrapMode::Clamp;
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

    //path to a directory
    static std::optional<ResourceHandle> loadTextureCube(const std::string& path)
    {
        auto textures = AssetManager::instance().loadDirectory<TextureAsset>(path);
        if (!textures.has_value()) { return std::nullopt; }

        std::vector<TextureFace> faces;
        for (auto& textureHandle : textures.value()) {
            const auto textureAsset = AssetManager::instance().get<TextureAsset>(textureHandle);
            auto& face = faces.emplace_back();
            face.width = textureAsset->getWidth();
            face.height = textureAsset->getHeight();
            face.channels = textureAsset->getChannels();
            face.pixels.resize(face.width * face.height * face.channels);
            memcpy(face.pixels.data(), textureAsset->getPixels(), face.width * face.height * face.channels);
        }
        auto cubeTexture = std::make_unique<TextureAsset>(faces, ColorSpace::SRGB, TextureType::TexCube);
        auto cubeAssetHandle = AssetManager::instance().add<TextureAsset>(std::move(cubeTexture));

        if (cubeAssetHandle.has_value()) {
            auto textureResource = std::make_unique<TextureResource>(cubeAssetHandle.value());
            return ResourceManager::instance().add<TextureResource>(std::move(textureResource));
        }
        return std::nullopt;
    }
}