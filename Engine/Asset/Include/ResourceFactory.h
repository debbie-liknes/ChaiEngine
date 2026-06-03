/**
 * @file ResourceFactory.h
 * @brief Interface for asset-specific backends
 */
#pragma once

#include "AssetCommon.h"

namespace chai
{
    /**
     * @brief Turns cooked asset bytes into a live GPU resource, and destroys it when the cache is
     * done. One per asset domain.
     * 
     * @description The cache owns the *policy* (when to load, refcount, dedup, lifetime); the
     * factory owns the *backend* (how to turn cooked bytes into a VkImage, how to destroy one).
     * Implement one concrete factory per asset domain
     */
    template <typename T>
    class ResourceFactory
    {
    public:
        using Asset = typename AssetTraits<T>::Asset;
        using Resource = typename AssetTraits<T>::Resource;

        virtual ~ResourceFactory() = default;

        /**
         * @brief Load + deserialize the CPU-side asset
         * @param id The asset to load
         * @param out The output asset. On failure, the cache will never call createResource
         * @return False if the load failed
         */
        [[nodiscard]] virtual bool loadAsset(AssetId id, Asset& out) = 0;

        /**
         * @brief Turn the CPU asset into a GPU resource
         * @param asset The CPU asset to turn into a GPU resource
         * @param out The output resource
         * @return Ready if the resource is ready to use immediately, Uploading if it's still in
         * flight and needs to be polled, or Failed if the asset was invalid and can't be turned
         * into a resource
         */
        [[nodiscard]] virtual LoadState createResource(const Asset& asset, Resource& out) = 0;

        /**
         * @brief Poll for an in flight upload
         * @param res The resource to poll
         * @return Ready if the resource is ready to use
         */
        [[nodiscard]] virtual LoadState pollState(const Resource&)
        {
            return LoadState::Ready;
        }

        /**
         * @brief Destroy the GPU resource
         * @param res The resource to poll
         * @warning Never call this directly on release
         */
        virtual void destroyResource(Resource& res) noexcept = 0;

        /**
         * @brief Whether the CPU asset copy can be discarded after upload
         * @return True if the cache can discard the CPU asset copy after upload
         */
        [[nodiscard]] virtual bool discardAssetAfterUpload() const noexcept { return true; }
    };

} // namespace chai