/**
 * @file IMeshRegistry.h
 */
#pragma once
#include <AssetCache.h>
#include <Handle.h>
#include <Geometry/Vertex.h>
#include <vector>
#include <Assets/MeshAsset.h>

namespace chai::gfx
{
    /**
     * @brief Bookkeeping for the Mesh Assets
     */
    class IMeshRegistry
    {
    public:
        virtual ~IMeshRegistry() = default;

        virtual Handle<Mesh> ingest(AssetId, MeshAsset) = 0;
        virtual Handle<Mesh> load(AssetId) = 0;
        virtual void release(Handle<Mesh>) = 0;
    };
} // namespace chai::gfx