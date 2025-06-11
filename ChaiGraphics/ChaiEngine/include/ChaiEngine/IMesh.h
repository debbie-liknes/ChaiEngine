#pragma once
#include <ChaiGraphicsExport.h>
#include <vector>
#include <memory>
#include <string>

namespace chai::brew
{
    struct Vertex;
    struct Color;

    class IMesh {
    public:
        virtual ~IMesh() = default;

        // Core interface
        virtual void bind() const = 0;
        virtual void unbind() const = 0;
        virtual uint32_t getVertexCount() const = 0;
        virtual uint32_t getIndexCount() const = 0;
        virtual bool isIndexed() const = 0;

        // Data access (for engine systems that need it)
        virtual const std::vector<Vertex>& getVertices() const = 0;
        virtual const std::vector<uint32_t>& getIndices() const = 0;

        // Resource management
        virtual bool isUploaded() const = 0;
        virtual void markDirty() = 0;
    };

    class MeshAsset {
        std::unique_ptr<IMesh> mesh;
        std::string assetId;
        int refCount = 0;
    public:
        static std::shared_ptr<MeshAsset> load(const std::string& path);
        IMesh* getMesh() const { return mesh.get(); }
    };
}