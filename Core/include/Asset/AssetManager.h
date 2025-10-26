#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <optional>
#include <shared_mutex>
#include <Asset/AssetLoader.h>
#include <Asset/AssetHandle.h>
#include <Asset/AssetPool.h>

namespace chai
{
    class AssetManager 
    {
    public:
        static AssetManager& instance();
        void registerLoader(std::shared_ptr<IAssetLoader> loader);

        // Load or get existing asset by path
        template<typename T>
        std::optional<Handle> load(const std::string& path)
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

            std::shared_ptr<IAsset> asset_result;
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
            Handle handle;
            {
                std::unique_lock<std::shared_mutex> pool_lock(pool_mutex_);
                handle = pool_.insert(asset_result);  // overload taking shared_ptr<IAsset>
            }

            // Update caches
            {
                std::unique_lock<std::shared_mutex> cache_lock(cache_mutex_);
                path_cache_[path] = handle;
                type_handles_[std::type_index(typeid(Handle))].push_back(handle);
            }

            return handle;
        }

        template<typename T, typename Func>
        auto get(Handle handle, Func&& func) const
            -> std::conditional_t<
            std::is_void_v<std::invoke_result_t<Func, const T&>>,
            std::optional<std::monostate>,
            std::optional<std::invoke_result_t<Func, const T&>>
            >
        {
            using R = std::invoke_result_t<Func, const T&>;
            using Ret = std::conditional_t<std::is_void_v<R>, std::optional<std::monostate>, std::optional<R>>;

            std::shared_lock<std::shared_mutex> lock(pool_mutex_);
            if (const T* asset = pool_.template get<T>(handle)) 
            {
                if constexpr (std::is_void_v<R>) 
                {
                    std::invoke(std::forward<Func>(func), *asset);
                    return Ret{ std::in_place, std::monostate{} };
                }
                else 
                {
                    return Ret{ std::in_place, std::invoke(std::forward<Func>(func), *asset) };
                }
            }

            std::cerr << "Invalid handle, could not locate asset.\n";
            return std::nullopt;
        }

    private:
        AssetPool pool_;
        std::unordered_map<std::string, Handle> path_cache_;
        std::unordered_map<std::type_index, std::vector<Handle>> type_handles_;

        mutable std::shared_mutex pool_mutex_;
        mutable std::shared_mutex cache_mutex_;

        std::vector<std::shared_ptr<IAssetLoader>> m_loaders;

        std::string getExtension(const std::string& file);
    };
}