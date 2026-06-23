/**
 * @file ModelRegistry.h
 */
#pragma once
#include <Assets/IMaterialRegistry.h>
#include <Assets/IMeshRegistry.h>
#include <Assets/IModelRegistry.h>
#include <Assets/ITextureRegistry.h>
#include <Assets/ModelAsset.h>
#include <Plugin/ServiceLocator.h>
#include <Assets/DecodePool.h>

namespace chai::gfx
{
    /**
     * @brief Registry for models
     * @note Have my doubts this needs to be in Core
     */
    class ModelRegistry : public IModelRegistry
    {
    public:
        ModelRegistry(IMeshRegistry& meshes,
                      ITextureRegistry& textures,
                      IMaterialRegistry& materials,
                      ServiceLocator* locator);

        ~ModelRegistry() override;

        [[nodiscard]] std::shared_ptr<const ModelPrefab>
        load(AssetId id, const std::filesystem::path& path) override;

        void release(AssetId id) override;
        void releaseAll();
        void tick();

    private:
        struct Loaded {
            std::shared_ptr<ModelPrefab>    prefab;
            std::vector<Handle<Mesh>>       meshes;
            std::vector<Handle<Material>>   materials;
        };

        static AssetId subId(AssetId base, const std::string& suffix);
        Handle<Texture>
        ingestImage(AssetId modelId, const ModelAsset::ImageData& img, int idx, bool srgb);
        void releaseEntry(Loaded& entry);

        IMeshRegistry&      meshes_;
        ITextureRegistry&   textures_;
        IMaterialRegistry&  materials_;
        ServiceLocator*     locator_;
        DecodePool          pool_;
        std::unordered_map<std::uint64_t, Loaded> loaded_;
    };
} // namespace chai::gfx