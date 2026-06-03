#pragma once
#include <Resource/ResourcePool.h>
#include <ChaiEngine/ResourceFactory.h>

#include <Asset/AssetHandle.h>
#include <Asset/AssetManager.h>
#include <memory>
#include <shared_mutex>
#include <typeindex>
#include <unordered_map>

namespace chai
{
    class ResourceManager
    {
    public:
        static ResourceManager& instance();

        // Create a resource from an asset, deduping if one already exists
        // for this asset. ResourceT is the GPU-side type (e.g. MeshResource),
        // AssetT is inferred from the handle.
        template <typename ResourceT, typename AssetT>
        Handle<ResourceT> createFromAsset(Handle<AssetT> assetHandle)
        {
            // Dedup check
            {
                std::shared_lock lock(m_dedupMutex);
                auto& map = getDedupMap<ResourceT, AssetT>();
                auto it = map.find(assetHandle);
                if (it != map.end() && getPool<ResourceT>().isValid(it->second)) {
                    return it->second;
                }
            }

            // Build the resource. This is the one place that knows how to
            // convert AssetT -> ResourceT. See layer 2 for how this dispatches.
            auto resource = ResourceFactory<ResourceT>::build(assetHandle);
            if (!resource)
                return {};

            Handle<ResourceT> handle;
            {
                std::unique_lock lock(m_poolMutex);
                handle = getPool<ResourceT>().add(std::move(resource));
            }

            {
                std::unique_lock lock(m_dedupMutex);
                getDedupMap<ResourceT, AssetT>()[assetHandle] = handle;
            }
            return handle;
        }

        // Add a resource directly (for resources not backed by an asset:
        // material instances, render targets, procedural meshes, etc.)
        template <typename ResourceT>
        Handle<ResourceT> add(std::unique_ptr<ResourceT> resource)
        {
            std::unique_lock lock(m_poolMutex);
            return getPool<ResourceT>().add(std::move(resource));
        }

        template <typename ResourceT>
        ResourceT* get(Handle<ResourceT> handle)
        {
            std::shared_lock lock(m_poolMutex);
            return getPool<ResourceT>().get(handle);
        }

        template <typename ResourceT>
        void remove(Handle<ResourceT> handle)
        {
            std::unique_lock lock(m_poolMutex);
            getPool<ResourceT>().remove(handle);
            // Note: dedup map entry becomes stale but harmless — the
            // isValid() check on lookup will catch it and trigger a rebuild.
        }

    private:
        template <typename ResourceT>
        ResourcePool<ResourceT>& getPool()
        {
            auto key = std::type_index(typeid(ResourceT));
            auto it = m_pools.find(key);
            if (it == m_pools.end()) {
                auto pool = std::make_unique<ResourcePool<ResourceT>>();
                auto* raw = pool.get();
                m_pools.emplace(key, std::move(pool));
                return *raw;
            }
            return *static_cast<ResourcePool<ResourceT>*>(it->second.get());
        }

        // Dedup map: (ResourceT, AssetT) -> (AssetHandle -> ResourceHandle).
        // Keyed by the pair of type_indices so Handle<MeshAsset> -> Handle<MeshResource>
        // is a separate map from Handle<TextureAsset> -> Handle<TextureResource>.
        template <typename ResourceT, typename AssetT>
        std::unordered_map<Handle<AssetT>, Handle<ResourceT>>& getDedupMap()
        {
            // One static map per (ResourceT, AssetT) pair. Simple, no type erasure.
            static std::unordered_map<Handle<AssetT>, Handle<ResourceT>> map;
            return map;
        }

        std::unordered_map<std::type_index, std::unique_ptr<IPoolBase>> m_pools;
        mutable std::shared_mutex m_poolMutex;
        mutable std::shared_mutex m_dedupMutex;
    };
} // namespace chai