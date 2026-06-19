/**
 * @file MeshRegistry.h
 */
#pragma once
#include <Assets/IMeshRegistry.h>
#include "MeshFactory.h"
#include "Mesh.h"

namespace chai::gfx
{
    /**
     * @brief The registry allows the core side to add meshes to the cache. The Mesh cache belogns
     * to the renderer
     */
    class MeshRegistry final : public IMeshRegistry
    {
    public:
        MeshRegistry(MeshFactory& factory, DeferredDeleteQueue& graveyard);

        Handle<Mesh> ingest(AssetId id, MeshAsset asset) override;
        void release(Handle<Mesh> h) override;
        Handle<Mesh> load(AssetId) override;

        std::shared_ptr<AssetCache<Mesh>> cache();

    private:
        std::shared_ptr<AssetCache<Mesh>> cache_;
    };
}