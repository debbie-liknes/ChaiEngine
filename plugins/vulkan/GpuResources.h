#pragma once
#include "Mesh.h"
#include "VulkanContext.h"

namespace chai::gfx
{
    class GpuResources
    {
        MeshFactory meshFactory_;
        DeferredDeleteQueue graveyard_;
        std::shared_ptr<AssetCache<Mesh>> meshCache_;
    public:
        explicit GpuResources(VulkanContext& ctx)
            : meshFactory_{ctx, ctx.allocator()}, graveyard_{}
        {
            meshCache_ = std::make_shared<AssetCache<Mesh>>(meshFactory_, graveyard_);
        }
        ~GpuResources()
        {
            graveyard_.flushAll();
        }

        std::shared_ptr<AssetCache<Mesh>> meshCache() { return meshCache_; }
        DeferredDeleteQueue& graveyard() { return graveyard_; }
    };
}