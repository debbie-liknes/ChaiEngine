#include "GLTFLoader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/MaterialSystem.h>
#include <Graphics/TextureAsset.h>

namespace chai
{
    bool GLTFLoader::canLoad(const std::string& ext) const
    {
        return ext == "gltf" || ext == "glb";
    }

    AssetHandle createMaterial(const tinygltf::Material& mat,
                               const tinygltf::Model& gltf,
                               const std::vector<ResourceHandle>& textures)
    {
        auto matAsset = std::make_unique<MaterialAsset>(mat.name, MaterialSystem::instance().getPBRShader());

        auto& pbr = mat.pbrMetallicRoughness;

        matAsset->setParameter(
            "u_albedo",
            Vec3(pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2]));

        matAsset->setFloat("u_metallic", pbr.metallicFactor);
        matAsset->setFloat("u_roughness", pbr.roughnessFactor);

        if (pbr.baseColorTexture.index >= 0) {
            int imageIndex = gltf.textures[pbr.baseColorTexture.index].source;
            matAsset->setParameter("u_albedoMap", textures[imageIndex]);
        } else {
            matAsset->setParameter("u_albedoMap", getDefaultWhiteTexture());
        }

        if (pbr.metallicRoughnessTexture.index >= 0) {
            int imageIndex = gltf.textures[pbr.metallicRoughnessTexture.index].source;
            matAsset->setParameter("u_metallicMap", textures[imageIndex]);
        } else {
            matAsset->setParameter("u_metallicMap", getDefaultWhiteTexture());
            //matAsset->setParameter("u_metallicRoughnessMap", getDefaultWhiteTexture());
        }

        if (mat.normalTexture.index >= 0) {
            int imageIndex = gltf.textures[mat.normalTexture.index].source;
            matAsset->setParameter("u_roughnessMap", textures[imageIndex]);
            //matAsset->setParameter("u_normalMap", textures[imageIndex]);
        } else {
            matAsset->setParameter("u_roughnessMap", getDefaultWhiteTexture());
            //matAsset->setParameter("u_normalMap", getDefaultWhiteTexture());
        }

        return AssetManager::instance().add<MaterialAsset>(std::move(matAsset)).value();
    }

    std::vector<Vec3> extractVec3(const tinygltf::Model& gltf, int accessorIndex)
    {
        auto& accessor = gltf.accessors[accessorIndex];
        auto& bufferView = gltf.bufferViews[accessor.bufferView];
        auto& buffer = gltf.buffers[bufferView.buffer];

        const uint8_t* base = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

        std::vector<Vec3> result(accessor.count);

        size_t stride = bufferView.byteStride ? bufferView.byteStride : sizeof(Vec3);

        for (size_t i = 0; i < accessor.count; i++) {
            const float* ptr = reinterpret_cast<const float*>(base + i * stride);
            result[i] = Vec3(ptr[0], ptr[1], ptr[2]);
        }

        return result;
    }

    std::vector<Vec2> extractVec2(const tinygltf::Model& gltf, int accessorIndex)
    {
        auto& accessor = gltf.accessors[accessorIndex];
        auto& bufferView = gltf.bufferViews[accessor.bufferView];
        auto& buffer = gltf.buffers[bufferView.buffer];

        const uint8_t* base = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

        std::vector<Vec2> result(accessor.count);

        size_t stride = bufferView.byteStride ? bufferView.byteStride : sizeof(Vec2);

        for (size_t i = 0; i < accessor.count; i++) {
            const float* ptr = reinterpret_cast<const float*>(base + i * stride);
            result[i] = Vec2(ptr[0], ptr[1]);
        }

        return result;
    }

    std::vector<Vec4> extractVec4(const tinygltf::Model& gltf, int accessorIndex)
    {
        auto& accessor = gltf.accessors[accessorIndex];
        auto& bufferView = gltf.bufferViews[accessor.bufferView];
        auto& buffer = gltf.buffers[bufferView.buffer];

        const uint8_t* base = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

        std::vector<Vec4> result(accessor.count);

        size_t stride = bufferView.byteStride ? bufferView.byteStride : sizeof(Vec4);

        for (size_t i = 0; i < accessor.count; i++) {
            const float* ptr = reinterpret_cast<const float*>(base + i * stride);
            result[i] = Vec4(ptr[0], ptr[1], ptr[2], ptr[3]);
        }

        return result;
    }

    std::vector<uint32_t> extractIndices(const tinygltf::Model& gltf, int accessorIndex)
    {
        auto& accessor = gltf.accessors[accessorIndex];
        auto& bufferView = gltf.bufferViews[accessor.bufferView];
        auto& buffer = gltf.buffers[bufferView.buffer];

        const uint8_t* base = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

        std::vector<uint32_t> result(accessor.count);

        // Indices can be different sizes
        if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            const uint16_t* ptr = reinterpret_cast<const uint16_t*>(base);
            for (size_t i = 0; i < accessor.count; i++) {
                result[i] = ptr[i];
            }
        } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            const uint32_t* ptr = reinterpret_cast<const uint32_t*>(base);
            for (size_t i = 0; i < accessor.count; i++) {
                result[i] = ptr[i];
            }
        } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
            for (size_t i = 0; i < accessor.count; i++) {
                result[i] = base[i];
            }
        }

        return result;
    }

    MeshAsset::MeshData extractMeshData(const tinygltf::Primitive& primitive,
                                        const tinygltf::Model& gltf)
    {
        MeshAsset::MeshData mesh;

        // primitive.attributes.
        // Positions (required)
        if (primitive.attributes.count("POSITION")) {
            mesh.positions = extractVec3(gltf, primitive.attributes.at("POSITION"));
        }

        // Normals
        if (primitive.attributes.count("NORMAL")) {
            mesh.normals = extractVec3(gltf, primitive.attributes.at("NORMAL"));
        }

        // Texture coordinates
        if (primitive.attributes.count("TEXCOORD_0")) {
            mesh.uvs = extractVec2(gltf, primitive.attributes.at("TEXCOORD_0"));
        }

        // Tangents (needed for normal mapping)
        if (primitive.attributes.count("TANGENT")) {
            mesh.tangents = extractVec4(gltf, primitive.attributes.at("TANGENT"));
        }

        // Indices
        if (primitive.indices >= 0) {
            mesh.indices = extractIndices(gltf, primitive.indices);
        }

        return mesh;
    }

    std::unique_ptr<IAsset> GLTFLoader::load(const std::string& path)
    {
        std::filesystem::path filePath(path);
        const auto parentDir = filePath.parent_path();
        tinygltf::Model gltf;
        tinygltf::TinyGLTF loader;
        std::string err, warn;

        bool success = path.ends_with(".glb") ? loader.LoadBinaryFromFile(&gltf, &err, &warn, path)
                                              : loader.LoadASCIIFromFile(&gltf, &err, &warn, path);
        if (!warn.empty()) {
            std::cout << "GLTF Warning: " << warn << std::endl;
        }

        if (!success) {
            std::cerr << "Failed to load GLTF file: " << err << std::endl;
            return nullptr;
        }

        auto model = std::make_unique<ModelAsset>();

        // Load textures
        std::vector<ResourceHandle> textures;
        for (auto& image : gltf.images) {
            TextureFace face;
            face.width = image.width;
            face.height = image.height;
            face.channels = image.component;
            face.pixels = image.image;
            auto tex = std::make_unique<TextureAsset>(std::vector<TextureFace>{face});

            auto texHandle = AssetManager::instance().add<TextureAsset>(std::move(tex));
            if (texHandle.has_value()) {
                auto textureResource =
                    std::make_unique<TextureResource>(std::vector<TextureFace>{face});
                textures.push_back(
                    ResourceManager::instance().add<TextureResource>(std::move(textureResource)));
            }
        }

        // Load materials
        for (auto& mat : gltf.materials) {
            model->materials.push_back(createMaterial(mat, gltf, textures));
        }

        // Load meshes
        for (auto& m : gltf.meshes) {
            MeshAsset::MeshData combinedMeshData;
            std::vector<MeshAsset::Submesh> submeshes;

            uint32_t currentIndexOffset = 0;
            uint32_t currentVertexOffset = 0;

            for (auto& primitive : m.primitives) {
                MeshAsset::MeshData primitiveData = extractMeshData(primitive, gltf);

                for (uint32_t index : primitiveData.indices) {
                    combinedMeshData.indices.push_back(index + currentVertexOffset);
                }

                combinedMeshData.positions.insert(combinedMeshData.positions.end(),
                                                  primitiveData.positions.begin(),
                                                  primitiveData.positions.end());
                combinedMeshData.normals.insert(combinedMeshData.normals.end(),
                                                primitiveData.normals.begin(),
                                                primitiveData.normals.end());
                combinedMeshData.uvs.insert(
                    combinedMeshData.uvs.end(), primitiveData.uvs.begin(), primitiveData.uvs.end());
                combinedMeshData.tangents.insert(combinedMeshData.tangents.end(),
                                                 primitiveData.tangents.begin(),
                                                 primitiveData.tangents.end());

                AssetHandle materialHandle = primitive.material >= 0
                                                 ? model->materials[primitive.material]
                                                 : MaterialSystem::instance().getPhongMaterial();

                submeshes.push_back({currentIndexOffset,
                                     static_cast<uint32_t>(primitiveData.indices.size()),
                                     materialHandle});

                currentIndexOffset += static_cast<uint32_t>(primitiveData.indices.size());
                currentVertexOffset += static_cast<uint32_t>(primitiveData.positions.size());
            }

            auto meshAsset = std::make_unique<MeshAsset>(std::move(combinedMeshData));
            for (const auto& submesh : submeshes) {
                meshAsset->addSubmesh(submesh.indexOffset, submesh.indexCount, submesh.material);
            }

            // Add to asset manager and store handle in model
            auto meshHandle = AssetManager::instance().add<MeshAsset>(std::move(meshAsset));
            if (meshHandle.has_value()) {
                model->meshes.push_back(meshHandle.value());
            }
        }

        // Build node hierarchy
        for (size_t i = 0; i < gltf.nodes.size(); i++) {
            auto& gltfNode = gltf.nodes[i];

            ModelAsset::Node node;
            node.name = gltfNode.name;
            node.meshIndex = gltfNode.mesh;
            node.childIndices = gltfNode.children;

            // Extract transform
            if (gltfNode.matrix.size() == 16) {
                for (size_t j = 0; j < 4; j++) {
                    for (size_t k = 0; k < 4; k++) {
                        node.localTransform[k][j] = static_cast<float>(gltfNode.matrix[j]);
                    }
                }
            } else {
                Vec3 t(0.0f);
                Quat r(1.0f, 0.0f, 0.0f, 0.0f);
                Vec3 s(1.0f);

                if (gltfNode.translation.size() == 3) {
                    t = Vec3(
                        gltfNode.translation[0], gltfNode.translation[1], gltfNode.translation[2]);
                }
                if (gltfNode.rotation.size() == 4) {
                    r = Quat(static_cast<float>(gltfNode.rotation[3]),
                             static_cast<float>(gltfNode.rotation[0]),
                             static_cast<float>(gltfNode.rotation[1]),
                             static_cast<float>(gltfNode.rotation[2]));
                }
                if (gltfNode.scale.size() == 3) {
                    s = Vec3(gltfNode.scale[0], gltfNode.scale[1], gltfNode.scale[2]);
                }

                node.localTransform = translate(Mat4(1.0f), t) * r.toMat4() * scale(Mat4(1.0f), s);
            }

            model->nodes.push_back(node);
        }

        // Set parent indices
        for (size_t i = 0; i < model->nodes.size(); i++) {
            for (int childIdx : model->nodes[i].childIndices) {
                model->nodes[childIdx].parentIndex = static_cast<int>(i);
            }
        }

        return model;
    }
} // namespace chai