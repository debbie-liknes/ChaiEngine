/**
 * @file GpuResources.h
 */
#pragma once
#include "Mesh.h"
#include "VulkanContext.h"

namespace chai::gfx
{
    /**
     * @brief Resources that belong to the plugin lifetime
     */
    class GpuResources
    {
        MeshFactory meshFactory_;
        DeferredDeleteQueue graveyard_;
    public:
        explicit GpuResources(VulkanContext& ctx)
            : meshFactory_{ctx, ctx.allocator()}, graveyard_{}
        {
        }
        ~GpuResources()
        {
            graveyard_.flushAll();
        }

        DeferredDeleteQueue& graveyard() { return graveyard_; }
        MeshFactory& factory() { return meshFactory_; }
    };
}