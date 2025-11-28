#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <optional>
#include <shared_mutex>
#include <Asset/AssetLoader.h>
#include <Asset/AssetHandle.h>
#include <Resource/ResourcePool.h>

#include <filesystem>
#include <iostream>

namespace chai
{
    class AssetManager
    {
    public:
        static AssetManager& instance();
        ~AssetManager();
        void registerLoader(std::shared_ptr<IAssetLoader> loader);

        // Load or get existing asset by path
        template <typename T>
        std::optional<AssetHandle> load(const std::string& path)
        {
            //Read lock to check cache
            {
                std::shared_lock cache_lock(cache_mutex_);
                auto it = path_cache_.find(path);
                if (it != path_cache_.end())
                {
                    return it->second;
                }
            }

            // Resolve path & pick loader
            std::string searchPath = RESOURCE_PATH + path;
            const auto ext = getExtension(searchPath);

            std::unique_ptr<IAsset> asset_result;
            for (auto& loader : m_loaders)
            {
                if (loader->canLoad(ext))
                {
                    asset_result = loader->load(searchPath);
                    if (asset_result) break;
                }
            }
            if (!asset_result) return std::nullopt;

            // Insert into pool
            AssetHandle handle;
            {
                std::unique_lock<std::shared_mutex> pool_lock(pool_mutex_);
                handle = AssetHandle(pool_.add(std::move(asset_result)));
            }

            // Update caches
            {
                std::unique_lock<std::shared_mutex> cache_lock(cache_mutex_);
                path_cache_[path] = handle;
                type_handles_[std::type_index(typeid(AssetHandle))].push_back(handle);
            }

            return handle;
        }

        template <typename T>
        std::optional<std::vector<AssetHandle>> loadDirectory(const std::string& dirPath)
        {
            std::string searchPath = RESOURCE_PATH + dirPath;

            if (!std::filesystem::is_directory(searchPath)) {
                std::cerr << "Path :" << dirPath << " is not a directory. Provide a directory for loadDirectory\n";
                return std::nullopt;
            }
            std::vector<AssetHandle> loadedAssets;
            for (const auto & entry : std::filesystem::directory_iterator(searchPath)) {
                auto assetPath = dirPath + "/" + entry.path().filename().string();
                auto assetHandle = load<T>(assetPath);
                if (assetHandle.has_value()) {
                    loadedAssets.push_back(assetHandle.value());
                }
            }
            return loadedAssets;
        }

        template <class U>
            requires std::derived_from<U, IAsset>
        std::optional<AssetHandle> add(std::unique_ptr<U> asset)
        {
            AssetHandle handle;
            {
                std::unique_lock<std::shared_mutex> pool_lock(pool_mutex_);
                handle = AssetHandle(pool_.add(std::move(asset)));
            }

            return handle;
        }

        //DO NOT store the returned pointer, it may be invalidated
        template <typename T>
        const T* get(AssetHandle handle) const
        {
            return dynamic_cast<const T*>(pool_.get(handle));
        }

    private:
        ResourcePool<IAsset> pool_;
        CMap<std::string, AssetHandle> path_cache_;
        CMap<std::type_index, std::vector<AssetHandle>> type_handles_;

        mutable std::shared_mutex pool_mutex_;
        mutable std::shared_mutex cache_mutex_;

        std::vector<std::shared_ptr<IAssetLoader>> m_loaders;

        std::string getExtension(const std::string& file);
    };
}