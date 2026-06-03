#pragma once
#include <Resource/ResourcePool.h>

#include <Asset/AssetHandle.h>
#include <Asset/AssetLoader.h>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace chai
{
    struct IAssetStorage {
        virtual ~IAssetStorage() = default;
    };

    template <typename T>
    struct AssetStorage : IAssetStorage {
        ResourcePool<T> pool;
        std::unordered_map<std::string, Handle<T>> pathCache;
        // Later: reload callbacks, debug names, whatever else.
    };

    class AssetManager
    {
    public:
        ~AssetManager();
        static AssetManager& instance();
        void registerLoader(std::shared_ptr<IAssetLoader> loader);

        template <typename T>
        Handle<T> add(std::unique_ptr<T> asset)
        {
            return getPool<T>().add(std::move(asset));
        }

        template <typename T>
        const T* get(Handle<T> handle)
        {
            return getPool<T>().get(handle);
        }

        template <typename T>
        std::optional<Handle<T>> load(const std::string& path)
        {
            auto resolved = resolvePath(path);
            if (!resolved)
                return std::nullopt;

            auto& storage = getStorage<T>();
            {
                std::shared_lock lock(m_mutex);
                auto it = storage.pathCache.find(*resolved);
                if (it != storage.pathCache.end() && storage.pool.isValid(it->second)) {
                    return it->second;
                }
            }

            const auto ext = getExtension(*resolved);
            std::unique_ptr<T> asset_result;

            for (auto& loader : m_loaders) {
                if (!loader->canLoad(ext)) {
                    continue;
                }

                // Type safety check: does this loader produce T?
                if (loader->assetType() != std::type_index(typeid(T))) {
                    // Loader handles this extension but for a different asset type.
                    // Not an error — skip and keep looking.
                    continue;
                }

                auto erased = loader->loadErased(*resolved);
                if (!erased)
                    continue;

                // Safe to cast: we verified the type_index matches.
                // Release from the erased unique_ptr, take ownership as T*.
                T* raw = static_cast<T*>(erased.release());
                auto typed = std::unique_ptr<T>(raw);

                std::unique_lock lock(m_mutex);
                auto handle = storage.pool.add(std::move(typed));
                storage.pathCache[*resolved] = handle;
                return handle;
            }

            return std::nullopt;
        }

        void addSearchPath(const std::string& path)
        {
            std::unique_lock lock(search_paths_mutex_);
            search_paths_.push_back(path);
        }

    private:

        template <typename T>
        AssetStorage<T>& getStorage()
        {
            auto key = std::type_index(typeid(T));
            auto it = m_storage.find(key);
            if (it == m_storage.end()) {
                auto s = std::make_unique<AssetStorage<T>>();
                auto* raw = s.get();
                m_storage.emplace(key, std::move(s));
                return *raw;
            }
            return static_cast<AssetStorage<T>&>(*it->second);
        }

        std::optional<std::string> resolvePath(const std::string& path)
        {
            // If it's an absolute path and exists, use it directly
            if (isAbsolutePath(path) && fileExists(path)) {
                return path;
            }

            // Search through registered paths
            std::shared_lock lock(search_paths_mutex_);
            for (const auto& searchPath : search_paths_) {
                std::string fullPath = joinPath(searchPath, path);
                if (fileExists(fullPath)) {
                    return fullPath;
                }
            }

            return std::nullopt;
        }

        bool isAbsolutePath(const std::string& path)
        {
            if (path.empty())
                return false;
#ifdef _WIN32
            return path.size() >= 2 && path[1] == ':';
#else
            return path[0] == '/';
#endif
        }

        std::string joinPath(const std::string& base, const std::string& relative)
        {
            if (base.empty())
                return relative;
            if (base.back() == '/' || base.back() == '\\') {
                return base + relative;
            }
            return base + "/" + relative;
        }

        bool fileExists(const std::string& path) { return std::filesystem::exists(path); }
        std::string getExtension(const std::string& file);

        std::unordered_map<std::type_index, std::unique_ptr<IAssetStorage>> m_storage;
        mutable std::shared_mutex m_mutex;

        // Type-erased storage of typed pools.
        // Each entry is actually a ResourcePool<SomeConcreteType>*.
        std::vector<std::shared_ptr<IAssetLoader>> m_loaders;
        std::vector<std::string> search_paths_;
        mutable std::shared_mutex search_paths_mutex_;
        mutable std::shared_mutex pool_mutex_;
        mutable std::shared_mutex cache_mutex_;
    };
} // namespace chai