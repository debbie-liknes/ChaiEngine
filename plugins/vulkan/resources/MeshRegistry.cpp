#include "MeshRegistry.h"

namespace chai::gfx
{
    MeshRegistry::MeshRegistry(MeshFactory& factory, DeferredDeleteQueue& graveyard)
    {
        cache_ = std::make_shared<AssetCache<Mesh>>(factory, graveyard);
    }

    Handle<Mesh> MeshRegistry::ingest(AssetId id, MeshAsset asset) 
    {
        return cache_->ingest(id, std::move(asset)); // forward to the real cache
    }

    void MeshRegistry::release(Handle<Mesh> h) 
    {
        cache_->release(h);
    }

    Handle<Mesh> MeshRegistry::load(AssetId) 
    {
        return {};
    }

    std::shared_ptr<AssetCache<Mesh>> MeshRegistry::cache()
    {
        return cache_;
    }
}