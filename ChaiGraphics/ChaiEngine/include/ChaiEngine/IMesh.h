#pragma once
#include <ChaiEngine/Vertex.h>
#include <Resource/Resource.h>
#include <ChaiEngine/Material.h>

#include <Asset/AssetHandle.h>
#include <Asset/AssetLoader.h>
#include <ChaiGraphicsExport.h>
#include <string>
#include <vector>
#include <Graphics/VertexAttribute.h>

namespace chai
{
    class MaterialAsset;

    struct Submesh {
        uint32_t indexOffset;
        uint32_t indexCount;
        Handle<MaterialAsset> material{};
    };

    class CHAIGRAPHICS_EXPORT MeshAsset
    {
    public:
        struct MeshData {
            std::vector<Vec3> positions;
            std::vector<Vec3> normals;
            std::vector<uint32_t> indices;
            std::vector<Vec2> uvs;
            std::vector<Vec4> tangents;
        };

        explicit MeshAsset(const MeshData& mesh) : m_meshData(mesh) { }
        ~MeshAsset() = default;

        const MeshData& getMeshData() const { return m_meshData; }
        const std::vector<Submesh>& getSubmeshes() const { return m_submeshes; }

        void addSubmesh(uint32_t offset, uint32_t count, Handle<MaterialAsset> material)
        {
            m_submeshes.emplace_back(offset, count, material);
        }

    private:
        MeshData m_meshData;
        std::vector<Submesh> m_submeshes;
    };

    struct CHAIGRAPHICS_EXPORT MeshResource {

        Handle<MeshAsset> sourceAsset;
        VertexLayout vertexLayout;
        std::vector<uint8_t> vertexData;
        uint32_t vertexCapacity = 0;
        uint32_t vertexCount = 0;
        std::vector<uint32_t> indexData;
        std::vector<Submesh> submeshes;

        explicit MeshResource(Handle<MeshAsset> source) : sourceAsset(source)
        {
        }
        MeshResource() = default;
    };

    struct ModelAsset {
        struct Node {
            std::string name;
            Mat4 localTransform = Mat4(1.0f);
            int meshIndex = -1;
            int parentIndex = -1;
            std::vector<int> childIndices;
        };

        std::vector<Node> nodes;
        std::vector<Handle<MeshAsset>> meshes;
        std::vector<Handle<MaterialAsset>> materials;

        //bool isValid() const override { return !meshes.empty(); }
        //const std::string& getAssetId() const override { return m_assetId; }

    private:
        std::string m_assetId;
    };
}