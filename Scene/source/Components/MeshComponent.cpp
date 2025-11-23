#include <Components/MeshComponent.h>
#include <Resource/ResourceManager.h>

namespace chai::cup
{
    MeshComponent::MeshComponent(GameObject* owner) : RenderableComponent(owner)
    {
    }

    MeshComponent::~MeshComponent()
    {
    }

    void MeshComponent::setMesh(AssetHandle meshAsset)
    {
        if (meshAsset.isValid())
        {
            m_meshResource = createMeshResourceFromAsset(AssetManager::instance().get<MeshAsset>(meshAsset));
        }
    }

    ResourceHandle MeshComponent::getMeshResource()
    {
        return m_meshResource;
    }

    void MeshComponent::setMaterial(ResourceHandle materialInstance)
    {
        m_materialInstance = materialInstance;
    }

    ResourceHandle MeshComponent::createMeshResourceFromAsset(const MeshAsset* asset)
    {
        auto resource = std::make_unique<MeshResource>();

        VertexLayout vertexLayout;

        if (!asset->getPositions().empty())
        {
            vertexLayout.addAttribute("a_Position", AttributeType::Float3);
        }

        if (!asset->getNormals().empty())
        {
            vertexLayout.addAttribute("a_Normal", AttributeType::Float3);
        }

        if (!asset->getUVs().empty())
        {
            vertexLayout.addAttribute("a_TexCoord", AttributeType::Float2);
        }

        resource->vertexLayout = vertexLayout;

        //interleave the data
        uint32_t stride = vertexLayout.getStride();
        uint32_t vertexCount = asset->getPositions().size();

        std::vector<uint8_t> vertexData(vertexCount * stride);

        for (size_t i = 0; i < vertexCount; ++i)
        {
            uint8_t* vertexDataPtr = &vertexData[i * stride];

            for (const auto& attr : vertexLayout.getAttributes())
            {
                if (attr.name == "a_Position")
                {
                    memcpy(vertexDataPtr + attr.offset, &asset->getPositions()[i], sizeof(Vec3));
                }
                else if (attr.name == "a_Normal" && i < asset->getNormals().size())
                {
                    memcpy(vertexDataPtr + attr.offset, &asset->getNormals()[i], sizeof(Vec3));
                }
                else if (attr.name == "a_TexCoord" && i < asset->getUVs().size())
                {
                    memcpy(vertexDataPtr + attr.offset, &asset->getUVs()[i], sizeof(Vec2));
                }
            }
        }

        resource->vertexData = std::move(vertexData);
        resource->vertexCount = vertexCount;

        if (!asset->getIndices().empty())
        {
            resource->indexData = asset->getIndices();
        }

        return ResourceManager::instance().add(std::move(resource));
    }
}