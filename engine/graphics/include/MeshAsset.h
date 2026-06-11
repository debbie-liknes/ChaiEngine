/**
 * @file MeshAsset.h
 */
#pragma once
#include <vector>
#include <Vertex.h>
#include <Handle.h>
#include <AssetCache.h>

namespace chai::gfx
{
    /**
     * @brief Emoty tag
     */
    struct Mesh;

     /**
     * @brief CPU side data for a mesh
     */
    struct MeshAsset 
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        bool empty() const { return vertices.empty() || indices.empty(); }

        bool isValid() const
        {
            if (empty() || indices.size() % 3 != 0)
                return false;
            for (uint32_t i : indices)
                if (i >= vertices.size())
                    return false;
            return true;
        }
    };

    /**
     * @brief Registries know how to create resources from assets.
     * Intended to be implemented in renderer plugin
     */
    class IMeshRegistry
    {
    public:
        virtual ~IMeshRegistry() = default;

        virtual Handle<Mesh> ingest(AssetId, MeshAsset) = 0;
        virtual Handle<Mesh> load(AssetId) = 0;
        virtual void release(Handle<Mesh>) = 0;
    };
}