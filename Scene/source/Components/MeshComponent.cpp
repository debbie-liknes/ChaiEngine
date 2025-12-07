#include "Components/TransformComponent.h"
#include "Scene/GameObject.h"

#include <ChaiEngine/Material.h>
#include <Resource/ResourceManager.h>

#include <Components/MeshComponent.h>
#include <span>

namespace chai::cup
{
    MeshComponent::MeshComponent(GameObject* owner) : RenderableComponent(owner) {}

    MeshComponent::~MeshComponent() {}

    void MeshComponent::setMesh(AssetHandle meshAsset)
    {
        if (meshAsset.isValid()) {
            auto asset = AssetManager::instance().get<MeshAsset>(meshAsset);
            m_meshResource = createMeshResourceFromAsset(asset);

            for (int i = 0; i < asset->getSubmeshes().size(); i++) {
                auto& submesh = asset->getSubmeshes()[i];
                auto matResource = createMaterialResourceFromAsset(submesh.material);
                m_materialOverrides.try_emplace(i, matResource);
            }
            recalculateAABB();
        }
    }

    void MeshComponent::setMaterial(ResourceHandle material)
    {
        m_singleMaterial = material;
        m_useSingleMaterial = true;
    }

    void MeshComponent::setMaterial(AssetHandle material)
    {
        m_singleMaterial = createMaterialResourceFromAsset(material);
        m_useSingleMaterial = true;
    }

    void MeshComponent::setMaterialInstance(ResourceHandle materialInstance)
    {
        m_materialInstance = materialInstance;
        m_useSingleMaterial = true;
    }

    void MeshComponent::setMaterial(size_t submeshIndex, AssetHandle material)
    {
        m_materialOverrides[submeshIndex] = createMaterialResourceFromAsset(material);
    }

    void MeshComponent::setMaterial(size_t submeshIndex, ResourceHandle material)
    {
        m_materialOverrides[submeshIndex] = material;
    }

    ResourceHandle MeshComponent::createMeshResourceFromAsset(const MeshAsset* asset)
    {
        auto resource = std::make_unique<MeshResource>();
        const auto& meshData = asset->getMeshData();

        VertexLayout vertexLayout;
        if (!meshData.positions.empty()) {
            vertexLayout.addAttribute("a_Position", DataType::Float3);
        }
        if (!meshData.normals.empty()) {
            vertexLayout.addAttribute("a_Normal", DataType::Float3);
        }
        if (!meshData.uvs.empty()) {
            vertexLayout.addAttribute("a_TexCoord", DataType::Float2);
        }
        if (!meshData.tangents.empty()) {
            vertexLayout.addAttribute("a_Tangent", DataType::Float4);
        }

        resource->vertexLayout = vertexLayout;

        uint32_t stride = vertexLayout.getStride();
        uint32_t vertexCount = static_cast<uint32_t>(meshData.positions.size());
        std::vector<uint8_t> vertexData(vertexCount * stride);

        for (size_t i = 0; i < vertexCount; ++i) {
            uint8_t* ptr = &vertexData[i * stride];
            for (const auto& attr : vertexLayout.getAttributes()) {
                if (attr.name == "a_Position") {
                    memcpy(ptr + attr.offset, &meshData.positions[i], sizeof(Vec3));
                } 
                else if (attr.name == "a_Normal" && i < meshData.normals.size()) {
                    memcpy(ptr + attr.offset, &meshData.normals[i], sizeof(Vec3));
                } 
                else if (attr.name == "a_TexCoord" && i < meshData.uvs.size()) {
                    memcpy(ptr + attr.offset, &meshData.uvs[i], sizeof(Vec2));
                } 
                else if (attr.name == "a_Tangent" && i < meshData.tangents.size()) {
                    memcpy(ptr + attr.offset, &meshData.tangents[i], sizeof(Vec4));
                }
            }
        }

        resource->vertexData = std::move(vertexData);
        resource->vertexCount = vertexCount;

        if (!meshData.indices.empty()) {
            resource->indexData = meshData.indices;
        }

        for (const auto& submesh : asset->getSubmeshes()) {
            resource->submeshes.push_back(
                {submesh.indexOffset, submesh.indexCount, submesh.material});
        }

        if (resource->submeshes.empty() && !resource->indexData.empty()) {
            resource->submeshes.push_back(
                {0, static_cast<uint32_t>(resource->indexData.size()), AssetHandle{}});
        }

        return ResourceManager::instance().add(std::move(resource));
    }

    ResourceHandle MeshComponent::createMeshResourceFromAsset(AssetHandle asset)
    {
        return createMeshResourceFromAsset(AssetManager::instance().get<MeshAsset>(asset));
    }

    ResourceHandle MeshComponent::createMaterialResourceFromAsset(AssetHandle assetHandle)
    {
        auto asset = AssetManager::instance().get<MaterialAsset>(assetHandle);
        auto resource = std::make_unique<MaterialResource>();
        resource->shaderAsset = asset->getShaderHandle();
        const auto& params = asset->getParameters();

        for (auto& [name, value] : params) {
            if (std::holds_alternative<ResourceHandle>(value)) {
                resource->textures[name] = {std::get<ResourceHandle>(value), /*slot=*/0};
            } else {
                resource->uniforms[name] = value;
            }
        }

        return ResourceManager::instance().add(std::move(resource));
    }

    bool MeshComponent::isVisible(const Frustum& frustum)
    {
        return frustum.isVisible(m_aabb);
    }
    void MeshComponent::update(double deltaTime)
    {
        if (getGameObject()->getComponent<TransformComponent>()->dirty()) {
            recalculateAABB();
        }
    }

    void MeshComponent::recalculateAABB()
    {
        m_aabb.min = Vec3(FLT_MAX);
        m_aabb.max = Vec3(-FLT_MAX);
        if (m_meshResource.isValid()) {
            auto resource = ResourceManager::instance().getResource<MeshResource>(m_meshResource);
            if (resource) {
                auto posAttr = resource->vertexLayout.findAttribute("a_Position");
                size_t stride = resource->vertexLayout.getStride();
                const uint8_t* data = resource->vertexData.data();

                Mat4 worldMat = Mat4::identity();
                auto *trans = getGameObject()->getComponent<TransformComponent>();
                if (trans != nullptr) {
                    worldMat = trans->getWorldMatrix();
                }

                for (size_t i = 0; i < resource->vertexCount; i++) {
                    const Vec3* pos = reinterpret_cast<const Vec3*>(
                        data + i * stride + posAttr->offset);

                    auto worldPos = worldMat * Vec4(*pos, 1.0f);
                    auto worldPos3 = Vec3(worldPos.x, worldPos.y, worldPos.z);
                    m_aabb.min = minVec(m_aabb.min, worldPos3);
                    m_aabb.max = maxVec(m_aabb.max, worldPos3);
                }
                printf("World matrix:\n");
                printf("  %f %f %f %f\n", worldMat[0][0], worldMat[1][0], worldMat[2][0], worldMat[3][0]);
                printf("  %f %f %f %f\n", worldMat[0][1], worldMat[1][1], worldMat[2][1], worldMat[3][1]);
                printf("  %f %f %f %f\n", worldMat[0][2], worldMat[1][2], worldMat[2][2], worldMat[3][2]);
                printf("  %f %f %f %f\n", worldMat[0][3], worldMat[1][3], worldMat[2][3], worldMat[3][3]);

                printf("AABB min: %f %f %f\n", m_aabb.min.x, m_aabb.min.y, m_aabb.min.z);
                printf("AABB max: %f %f %f\n", m_aabb.max.x, m_aabb.max.y, m_aabb.max.z);
                printf("Stride: %zu, Position offset: %zu\n", stride, posAttr->offset);
            }
        }
    }
}