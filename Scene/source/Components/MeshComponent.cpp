#include <ChaiEngine/ResourceManager.h>

#include <Components/MeshComponent.h>
#include <ChaiEngine/Material.h>

namespace chai::scene
{
    //MeshComponent::MeshComponent(GameObject* owner) : RenderableComponent(owner) {}

    //MeshComponent::~MeshComponent() {}

    //void MeshComponent::setMesh(Handle<MeshAsset> meshAsset)
    //{
    //    //if (!meshAsset.isNull()) {
    //    //    auto asset = AssetManager::instance().get<MeshAsset>(meshAsset);
    //    //    m_meshResource = createMeshResourceFromAsset(asset);

    //    //    for (int i = 0; i < asset->getSubmeshes().size(); i++) {
    //    //        auto& submesh = asset->getSubmeshes()[i];
    //    //        auto matResource = createMaterialResourceFromAsset(submesh.material);
    //    //        m_materialOverrides.try_emplace(i, matResource);
    //    //    }
    //    //}
    //}

    //void MeshComponent::setMaterial(Handle<MaterialResource> material)
    //{
    //    m_singleMaterial = material;
    //    m_useSingleMaterial = true;
    //}

    //void MeshComponent::setMaterial(Handle<MaterialAsset> material)
    //{
    //    m_singleMaterial = createMaterialResourceFromAsset(material);
    //    m_useSingleMaterial = true;
    //}

    //void MeshComponent::setMaterialInstance(Handle<MaterialResource> materialInstance)
    //{
    //    m_materialInstance = materialInstance;
    //    m_useSingleMaterial = true;
    //}

    //void MeshComponent::setMaterial(size_t submeshIndex, Handle<MaterialAsset> material)
    //{
    //    m_materialOverrides[submeshIndex] = createMaterialResourceFromAsset(material);
    //}

    //void MeshComponent::setMaterial(size_t submeshIndex, Handle<MaterialResource> material)
    //{
    //    m_materialOverrides[submeshIndex] = material;
    //}

    //Handle<MeshResource> MeshComponent::createMeshResourceFromAsset(const MeshAsset* asset)
    //{
    //    //auto resource = std::make_unique<MeshResource>();
    //    //const auto& meshData = asset->getMeshData();

    //    //VertexLayout vertexLayout;
    //    //if (!meshData.positions.empty()) {
    //    //    vertexLayout.addAttribute("a_Position", DataType::Float3);
    //    //}
    //    //if (!meshData.normals.empty()) {
    //    //    vertexLayout.addAttribute("a_Normal", DataType::Float3);
    //    //}
    //    //if (!meshData.uvs.empty()) {
    //    //    vertexLayout.addAttribute("a_TexCoord", DataType::Float2);
    //    //}
    //    //if (!meshData.tangents.empty()) {
    //    //    vertexLayout.addAttribute("a_Tangent", DataType::Float4);
    //    //}

    //    //resource->vertexLayout = vertexLayout;

    //    //uint32_t stride = vertexLayout.getStride();
    //    //uint32_t vertexCount = static_cast<uint32_t>(meshData.positions.size());
    //    //std::vector<uint8_t> vertexData(vertexCount * stride);

    //    //for (size_t i = 0; i < vertexCount; ++i) {
    //    //    uint8_t* ptr = &vertexData[i * stride];
    //    //    for (const auto& attr : vertexLayout.getAttributes()) {
    //    //        if (attr.name == "a_Position") {
    //    //            memcpy(ptr + attr.offset, &meshData.positions[i], sizeof(Vec3));
    //    //        } 
    //    //        else if (attr.name == "a_Normal" && i < meshData.normals.size()) {
    //    //            memcpy(ptr + attr.offset, &meshData.normals[i], sizeof(Vec3));
    //    //        } 
    //    //        else if (attr.name == "a_TexCoord" && i < meshData.uvs.size()) {
    //    //            memcpy(ptr + attr.offset, &meshData.uvs[i], sizeof(Vec2));
    //    //        } 
    //    //        else if (attr.name == "a_Tangent" && i < meshData.tangents.size()) {
    //    //            memcpy(ptr + attr.offset, &meshData.tangents[i], sizeof(Vec4));
    //    //        }
    //    //    }
    //    //}

    //    //resource->vertexData = std::move(vertexData);
    //    //resource->vertexCount = vertexCount;

    //    //if (!meshData.indices.empty()) {
    //    //    resource->indexData = meshData.indices;
    //    //}

    //    //for (const auto& submesh : asset->getSubmeshes()) {
    //    //    resource->submeshes.push_back(
    //    //        {submesh.indexOffset, submesh.indexCount, submesh.material});
    //    //}

    //    //if (resource->submeshes.empty() && !resource->indexData.empty()) {
    //    //    resource->submeshes.push_back(
    //    //        {0, static_cast<uint32_t>(resource->indexData.size()), Handle<MaterialAsset>{}});
    //    //}

    //    //return ResourceManager::instance().add(std::move(resource));
    //    return Handle<MeshResource>{};
    //}

    //Handle<MeshResource> MeshComponent::createMeshResourceFromAsset(Handle<MeshAsset> asset)
    //{
    //    return createMeshResourceFromAsset(AssetManager::instance().get<MeshAsset>(asset));
    //}

    //Handle<MaterialResource>
    //MeshComponent::createMaterialResourceFromAsset(Handle<MaterialAsset> assetHandle)
    //{
    //    //auto asset = AssetManager::instance().get<MaterialAsset>(assetHandle);
    //    //auto resource = std::make_unique<MaterialResource>();
    //    //resource->shaderAsset = asset->getShaderHandle();
    //    //const auto& params = asset->getParameters();

    //    //for (auto& [name, value] : params) {
    //    //    //if (std::holds_alternative<Handle<MaterialResource>>(value)) {
    //    //    //    resource->textures[name] = {std::get<Handle<MaterialResource>>(value), /*slot=*/0};
    //    //    //} else {
    //    //    //    resource->uniforms[name] = value;
    //    //    //}
    //    //}

    //    //return ResourceManager::instance().add(std::move(resource));
    //    return Handle<MaterialResource>{};
    //}
}