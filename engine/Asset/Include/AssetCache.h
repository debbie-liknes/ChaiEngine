/**
 * @file AssetCache.h
 * @brief Manages asset lifecycles, refcounts, and deduplication
 */
#pragma once

#include "AssetCommon.h"
#include "DeferredDeleteQueue.h"
#include "Handle.h"
#include "ResourceFactory.h"
#include "SlotMap.h"

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace chai
{
    /**
     * @brief Policy layer for one asset domain.
     *
     * Lifetime model: assets are refcounted and de-duplicated by AssetId, so loading the same
     * content twice returns the same handle and bumps a count. When the last reference drops, the
     * slot is freed and its GPU resource is routed through the graveyard for deferred destruction
     */
    template <typename T>
    class AssetCache
    {
    public:
        using HandleType = Handle<T>;
        using Asset = typename AssetTraits<T>::Asset;
        using Resource = typename AssetTraits<T>::Resource;
        using Factory = ResourceFactory<T>;

        /**
         * @brief Creates an asset cache using the provided factory and delete queue.
         *
         * The factory and delete queue must outlive the cache.
         */
        AssetCache(Factory& factory, DeferredDeleteQueue& graveyard) noexcept;

        AssetCache(const AssetCache&) = delete;
        AssetCache& operator=(const AssetCache&) = delete;

        /**
         * @brief Acquire a reference to an asset by id.
         *
         * If it is already loaded, returns the existing handle and bumps its refcount
         * @return A handle to the asset
         */
        [[nodiscard]] HandleType acquire(AssetId id);

        /**
         * @brief Supply the asset to be cached
         */
        [[nodiscard]] auto ingest(AssetId id, Asset asset) -> HandleType;

        /**
         * @brief Add an explicit extra reference to an already-held handle
         * @return The same handle
         */
        HandleType addRef(HandleType h);

        /**
         * @brief Drop a reference. On the last reference the slot is freed
         */
        void release(HandleType h);

        /**
         * @brief Release all slots
         */
        void releaseAll();

        /**
         * @brief Check the load state of a handle
         * @return The current load state
         */
        [[nodiscard]] LoadState state(HandleType h) const;

        /**
         * @brief Check if a handle is ready to use
         * @return True if ready, false if still loading or failed
         */
        [[nodiscard]] bool isReady(HandleType h) const;

        /**
         * @brief Get a pointer to the GPU resource for a handle, or nullptr if not ready.
         * @param h The handle to query
         * @warning Do NOT cache the returned pointer! It is frame-local and may be invalidated
         * @return Pointer to the resource
         */
        // The live GPU resource, or nullptr if not Ready (bind the fallback then).
        // The CPU asset, or nullptr if not loaded / already discarded.
        // Both pointers are frame-local -- do not cache them, re-resolve each use.
        [[nodiscard]] const Resource* resource(HandleType h) const;

        /**
         * @brief Get a pointer to the CPU asset for a handle, or nullptr if not loaded.
         * @param h The handle to query
         * @warning Do NOT cache the returned pointer! It is frame-local and may be invalidated
         * @return Pointer to the asset
         */
        [[nodiscard]] const Asset* asset(HandleType h) const;

        /**
         * @brief Advance in-flight uploads. Call once per frame.
         */
        void tick();

        /**
         * @brief Rebuild the resource for an id in place.
         * @param id The id of the asset to reload
         */
        void reload(AssetId id);

        /**
         * @brief Get the number of live assets currently in the cache
         * @return The number of live assets currently in the cache
         */
        [[nodiscard]] std::size_t liveAssets() const noexcept { return slots_.size(); }

        /**
         * @brief Get the number of pending uploads
         * @return The number of pending uploads
         */
        [[nodiscard]] std::size_t pendingUploads() const noexcept { return uploading_.size(); }

    private:
        struct Record {
            AssetId id{};
            LoadState state = LoadState::Empty;
            std::uint32_t refCount = 0;
            Asset asset{};       // CPU
            Resource resource{}; // GPU
        };

        void startUpload(HandleType h);
        void promoteReady(Record& rec, LoadState s);
        void destroyResourceDeferred(Resource res);

        Factory* factory_;
        DeferredDeleteQueue* graveyard_;
        SlotMap<T, Record> slots_;
        std::unordered_map<std::uint64_t, HandleType> byId_;
        std::vector<HandleType> uploading_;
        std::vector<HandleType> queued_;
    };

    ///////////////////////////////// implementation below /////////////////////////////////
    template <typename T>
    AssetCache<T>::AssetCache(Factory& factory, DeferredDeleteQueue& graveyard) noexcept
        : factory_(&factory), graveyard_(&graveyard)
    {
    }

    template <typename T>
    auto AssetCache<T>::acquire(AssetId id) -> HandleType
    {
        // Check if already loaded
        if (auto it = byId_.find(id.value); it != byId_.end()) {
            if (Record* rec = slots_.get(it->second)) {
                // still live, increase ref and return it
                rec->refCount++;
                return it->second;
            }
            // stale
            byId_.erase(it);
        }

        // Not loaded yet, create a new slot. Refcount starts at 1
        Record rec{};
        rec.id = id;
        rec.refCount = 1;
        rec.state = LoadState::Loading;

        HandleType h = slots_.insert(std::move(rec));
        byId_.emplace(id.value, h);
        return h;
    }

    template <typename T>
    auto AssetCache<T>::ingest(AssetId id, Asset asset) -> HandleType
    {
        if (auto it = byId_.find(id.value); it != byId_.end()) {
            if (Record* rec = slots_.get(it->second)) {
                // fulfillment. do NOT touch refCount
                if (rec->state == LoadState::Loading) {
                    rec->asset = std::move(asset);
                    rec->state = LoadState::Queued;
                    queued_.push_back(it->second);
                }
                return it->second;
            }
            byId_.erase(it);
        }

        Record rec{};
        rec.id = id;
        rec.refCount = 1;
        rec.state = LoadState::Queued;
        rec.asset = std::move(asset);

        HandleType h = slots_.insert(std::move(rec));
        byId_.emplace(id.value, h);
        queued_.push_back(h);
        return h;
    }

    template <typename T>
    auto AssetCache<T>::addRef(HandleType h) -> HandleType
    {
        if (Record* rec = slots_.get(h))
            rec->refCount++;
        return h;
    }

    template <typename T>
    void AssetCache<T>::release(HandleType h)
    {
        Record* rec = slots_.get(h);
        if (!rec)
            return;
        if (rec->refCount > 0)
            rec->refCount--;
        if (rec->refCount != 0)
            return;

        // Last release. free the slot and GPU resource
        byId_.erase(rec->id.value);
        destroyResourceDeferred(
            std::move(rec->resource)); // safe way to destroy even mid-upload resources
        slots_.erase(h);
    }

    template <typename T>
    LoadState AssetCache<T>::state(HandleType h) const
    {
        const Record* rec = slots_.get(h);
        return rec ? rec->state : LoadState::Empty;
    }

    template <typename T>
    bool AssetCache<T>::isReady(HandleType h) const
    {
        const Record* rec = slots_.get(h);
        return rec && rec->state == LoadState::Ready;
    }

    template <typename T>
    auto AssetCache<T>::resource(HandleType h) const -> const Resource*
    {
        const Record* rec = slots_.get(h);
        return (rec && rec->state == LoadState::Ready) ? &rec->resource : nullptr;
    }

    template <typename T>
    auto AssetCache<T>::asset(HandleType h) const -> const Asset*
    {
        const Record* rec = slots_.get(h);
        return rec ? &rec->asset : nullptr;
    }

    template <typename T>
    void AssetCache<T>::tick()
    {
        const int kUploadsPerFrame = 16;
        int budget = kUploadsPerFrame;
        while (budget > 0 && !queued_.empty()) {
            HandleType h = queued_.front();
            queued_.erase(queued_.begin());
            if (slots_.get(h)) {            // still live?
                startUpload(h); // Queued -> Uploading | Ready | Failed
                --budget;
            }
        }

        //process uploading
        std::size_t w = 0;
        for (std::size_t r = 0; r < uploading_.size(); ++r) {
            HandleType h = uploading_[r];
            Record* rec = slots_.get(h);
            if (!rec)
                continue;

            LoadState s = factory_->pollState(rec->resource);
            if (s == LoadState::Uploading) {
                uploading_[w++] = h;
            } else {
                promoteReady(*rec, s);
            }
        }
        uploading_.resize(w);
    }

    template <typename T>
    void AssetCache<T>::reload(AssetId id)
    {
        auto it = byId_.find(id.value);
        if (it == byId_.end())
            return;

        // If it's still live, drop the old resource, and start a replacement
        Record* rec = slots_.get(it->second);
        if (!rec)
            return;

        destroyResourceDeferred(std::move(rec->resource));
        rec->resource = Resource{};
        rec->asset = Asset{};
        rec->state = LoadState::Loading;
    }

    template <typename T>
    void AssetCache<T>::startUpload(HandleType h)
    {
        Record* rec = slots_.get(h);
        if (!rec)
            return;

        LoadState s = factory_->createResource(rec->asset, rec->resource);
        if (s == LoadState::Uploading) {
            rec->state = LoadState::Uploading;
            uploading_.push_back(h); // tick() promotes it later
        } else {
            promoteReady(*rec, s);
        }
    }

    template <typename T>
    void AssetCache<T>::promoteReady(Record& rec, LoadState s)
    {
        rec.state = s;
        if (s == LoadState::Ready && factory_->discardAssetAfterUpload()) {
            rec.asset = Asset{}; // free the CPU copy
        }
    }

    template <typename T>
    void AssetCache<T>::destroyResourceDeferred(Resource res)
    {
        // TODO: is shared pointer the right move here?
        auto held = std::make_shared<Resource>(std::move(res));
        Factory* fac = factory_;
        graveyard_->enqueue([fac, held]() { fac->destroyResource(*held); });
    }

    template <typename T>
    void AssetCache<T>::releaseAll()
    {
        slots_.forEach([&](HandleType h, Record& rec) {
            destroyResourceDeferred(std::move(rec.resource));
            rec.resource = Resource{};
        });
        slots_.clear();
        byId_.clear();
        uploading_.clear();
        queued_.clear();
    }
} // namespace chai