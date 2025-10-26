#pragma once
#include <ChaiGraphicsExport.h>
#include <vector>
#include <memory>
#include <string>
#include <Resource/ResourceLoader.h>
#include <Resource/ResourceHandle.h>
#include <ChaiEngine/Vertex.h>
#include <Meta/ChaiMacros.h>

namespace chai
{
    class IMesh 
    {
    public:
        virtual ~IMesh() = default;

        virtual size_t getVertexCount() const = 0;
        virtual size_t getIndexCount() const = 0;

        // Data access (for engine systems that need it)
        virtual const std::vector<Vertex>& getVertices() const = 0;
        virtual const std::vector<uint32_t>& getIndices() const = 0;
    };

    class Mesh : public IMesh, public IAsset 
    {
    public:
        Mesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& inds)
            : vertices(verts), indices(inds) 
        {}

		virtual ~Mesh() = default;

        const std::vector<Vertex>& getVertices() const override { return vertices; }
        const std::vector<uint32_t>& getIndices() const override { return indices; }

        size_t getVertexCount() const override { return vertices.size(); }
        size_t getIndexCount() const override { return indices.size(); }

        void addMaterialLibrary(const std::string& mat) { m_materialLibraries.push_back(mat); }

        bool isValid() const override { return m_valid; }
        const std::string& getAssetId() const override { return m_assetId; }

    private:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        //want this
        //AABB boundingBox;

        std::shared_ptr<IMesh> m_mesh;
        std::string m_assetId;
        int m_refCount = 0;
        bool m_valid = false;
        std::vector<std::string> m_materialLibraries;
    };
}