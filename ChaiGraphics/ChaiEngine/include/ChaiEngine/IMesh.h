#pragma once
#include <ChaiGraphicsExport.h>
#include <vector>
#include <string>
#include <Asset/AssetLoader.h>
#include <Asset/AssetHandle.h>
#include <ChaiEngine/Vertex.h>
#include <Resource/Resource.h>

#include "Graphics/VertexAttribute.h"

namespace chai
{
    class CHAIGRAPHICS_EXPORT MeshAsset : public IAsset
    {
    public:
        struct MeshData {
            std::vector<Vec3> positons;
            std::vector<Vec3> normals;
            std::vector<uint32_t> indices;
            std::vector<Vec2> uvs;
        };

        explicit MeshAsset(const MeshData& mesh) : m_meshData(mesh) {
        }
        ~MeshAsset() override = default;

        const std::vector<Vec3>& getPositions() const { return m_meshData.positons; }
        const std::vector<Vec3>& getNormals() const { return m_meshData.normals; }
        const std::vector<uint32_t>& getIndices() const { return m_meshData.indices; }
        const std::vector<Vec2>& getUVs() const { return m_meshData.uvs; }

        const std::vector<AssetHandle>& getMaterials() const { return m_defaultMaterials; }

        void addMaterial(const AssetHandle mat) { m_defaultMaterials.push_back(mat); }

        bool isValid() const override { return m_valid; }
        const std::string& getAssetId() const override { return m_assetId; }

    private:
        const MeshData m_meshData;
        std::vector<AssetHandle> m_defaultMaterials;
        //want this
        //AABB boundingBox;

        int m_refCount = 0;
        bool m_valid = false;
    };

    struct CHAIGRAPHICS_EXPORT MeshResource : public Resource
    {
        AssetHandle sourceAsset;
        VertexLayout vertexLayout;
        std::vector<uint8_t> vertexData;
        uint32_t vertexCapacity = 0;
        uint32_t vertexCount = 0;
        std::vector<uint32_t> indexData;

        explicit MeshResource(AssetHandle source)
            : Resource(source), sourceAsset(source)
        {}
        MeshResource() = default;
    };
}
