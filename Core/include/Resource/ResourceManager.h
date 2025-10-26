#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <optional>
#include <shared_mutex>
#include <Resource/ResourceLoader.h>
#include <Resource/ResourceHandle.h>
#include <Resource/ResourcePool.h>

namespace chai
{
   // class ResourceManager 
   // {
   // public:
   //     static ResourceManager& instance();

   //     void registerLoader(std::shared_ptr<IResourceLoader> loader);

   //     template <typename T = IResource>
   //     std::shared_ptr<T> load(const std::string& path) 
   //     {
			//std::string searchPath = RESOURCE_PATH + path;
   //         // Check cache
			//// This implies that every resource is loaded on the main thread
			//// TODO: Add support for async loading
			//// Most large resource loads (images, models, etc.) should be done on a separate thread
   //         auto it = m_cache.find(searchPath);
   //         if (it != m_cache.end()) {
   //             return std::dynamic_pointer_cast<T>(it->second);
   //         }

   //         // Find loader
   //         auto ext = getExtension(searchPath);
   //         for (auto& loader : m_loaders) {
   //             if (loader->canLoad(ext)) {
   //                 auto resource = loader->load(searchPath);
   //                 m_cache[searchPath] = resource;
   //                 return std::dynamic_pointer_cast<T>(resource);
   //             }
   //         }

   //         throw std::runtime_error("No loader found for: " + searchPath);
   //     }

   // private:
   //     CMap<std::string, std::shared_ptr<IResource>> m_cache;
   //     std::vector<std::shared_ptr<IResourceLoader>> m_loaders;

   //     std::string getExtension(const std::string& file);
   // };

    class NewAssetManager 
    {
    public:
        static NewAssetManager& instance();
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

            // 2) Resolve path & pick loader
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

            // 3) Insert into pool (write lock)
            Handle handle;
            {
                std::unique_lock<std::shared_mutex> pool_lock(pool_mutex_);
                handle = pool_.insert(asset_result);  // overload taking shared_ptr<IAsset>
            }

            // 4) Update caches (write lock)
            {
                std::unique_lock<std::shared_mutex> cache_lock(cache_mutex_);
                path_cache_[path] = handle;
                type_handles_[std::type_index(typeid(Handle))].push_back(handle);
            }

            return handle;
        }


        // Insert asset directly without path
        //template<typename T>
        //auto insert(T asset) {
        //    std::type_index type_id = std::type_index(typeid(T));
        //    Handle handle;

        //    {
        //        std::unique_lock<std::shared_mutex> pool_lock(pool_mutex_);
        //        handle = pool_.insert<T>(std::move(asset));
        //    }

        //    {
        //        std::unique_lock<std::shared_mutex> cache_lock(cache_mutex_);
        //        type_handles_[type_id].push_back(handle);
        //    }

        //    return handle;
        //}

        // Get immutable access to asset
        //template<typename T, typename Func>
        //auto get(Handle handle, Func func) const -> std::optional<decltype(func(std::declval<const T&>()))> 
        //{
        //    std::shared_lock<std::shared_mutex> lock(pool_mutex_);
        //    if (const T* asset = pool_.get<T>(handle)) 
        //    {
        //        return func(*asset);
        //    }

        //    std::cerr << "Invalid handle, could not locate asset." << std::endl;
        //    return std::nullopt;
        //}

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

        //// Get mutable access to asset
        //template<typename T, typename Func>
        //auto get_mut(Handle handle, Func func) -> std::optional<decltype(func(std::declval<T&>()))> {
        //    std::unique_lock<std::shared_mutex> lock(pool_mutex_);
        //    T* asset = pool_.get_mut<T>(handle);
        //    if (asset) {
        //        return func(*asset);
        //    }
        //    return std::nullopt;
        //}

        //// Remove asset
        //template<typename T>
        //std::optional<T> remove(Handle handle) {
        //    std::unique_lock<std::shared_mutex> pool_lock(pool_mutex_);
        //    auto asset = pool_.remove<T>(handle);

        //    if (asset.has_value()) {
        //        std::unique_lock<std::shared_mutex> cache_lock(cache_mutex_);

        //        // Clean up path cache
        //        for (auto it = path_cache_.begin(); it != path_cache_.end();) {
        //            if (it->second == handle) {
        //                it = path_cache_.erase(it);
        //            }
        //            else {
        //                ++it;
        //            }
        //        }

        //        // Clean up type tracking
        //        auto& handles = type_handles_[handle.type];
        //        handles.erase(std::remove(handles.begin(), handles.end(), handle), handles.end());
        //    }

        //    return asset;
        //}

        //// Check if handle is valid
        //bool is_valid(Handle handle) const {
        //    std::shared_lock<std::shared_mutex> lock(pool_mutex_);
        //    return pool_.is_valid(handle);
        //}

        //// Get handle by path (if cached)
        //std::optional<Handle> get_handle(const std::string& path) const {
        //    std::shared_lock<std::shared_mutex> lock(cache_mutex_);
        //    auto it = path_cache_.find(path);
        //    if (it != path_cache_.end()) {
        //        return it->second;
        //    }
        //    return std::nullopt;
        //}

        //// Get all handles of a specific type
        //template<typename T>
        //std::vector<Handle> get_all_handles() const {
        //    std::type_index type_id = std::type_index(typeid(T));
        //    std::shared_lock<std::shared_mutex> lock(cache_mutex_);

        //    auto it = type_handles_.find(type_id);
        //    if (it != type_handles_.end()) {
        //        return it->second;
        //    }
        //    return {};
        //}

        //// Global operations
        //size_t total_asset_count() const {
        //    std::shared_lock<std::shared_mutex> lock(pool_mutex_);
        //    return pool_.size();
        //}

        //void clear_all() {
        //    std::unique_lock<std::shared_mutex> pool_lock(pool_mutex_);
        //    std::unique_lock<std::shared_mutex> cache_lock(cache_mutex_);

        //    pool_.clear();
        //    path_cache_.clear();
        //    type_handles_.clear();
        //}

        //// Unload all assets of a specific type
        //template<typename T>
        //void clear_type() {
        //    std::type_index type_id = std::type_index(typeid(T));

        //    std::vector<Handle> handles;
        //    {
        //        std::shared_lock<std::shared_mutex> cache_lock(cache_mutex_);
        //        auto it = type_handles_.find(type_id);
        //        if (it != type_handles_.end()) {
        //            handles = it->second;
        //        }
        //    }

        //    for (const auto& handle : handles) {
        //        remove<T>(handle);
        //    }
        //}

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