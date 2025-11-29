#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "ObjLoader.h"
#include <iostream>
#include <format>
#include <ChaiEngine/Material.h>
#include <ChaiEngine/IMesh.h>
#include <Types/CMap.h>
#include <filesystem>
#include <ChaiMath.h>

namespace chai
{
    bool ObjLoader::canLoad(const std::string& ext) const
    {
        return ext == "obj";
    }

    struct OBJVertex
    {
        Vec3 position;
        Vec3 normal;
        Vec2 uv;
    };

    std::unique_ptr<IAsset> ObjLoader::load(const std::string& path)
    {
        tinyobj::ObjReaderConfig reader_config;
        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(path, reader_config))
        {
            if (!reader.Error().empty())
            {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            return nullptr; // Don't exit, return null
        }

        if (!reader.Warning().empty())
        {
            std::cout << "TinyObjReader: " << reader.Warning();
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        auto& materials = reader.GetMaterials();

        // Create mesh data containers
        std::vector<OBJVertex> vertices;
        std::vector<uint32_t> indices;
        CMap<std::string, uint32_t> uniqueVertices;

        uint32_t currentIndex = 0;


        // Loop over shapes
        for (auto& shape : shapes)
        {
            size_t indexOffset = 0;
            for (auto& numVerts : shape.mesh.num_face_vertices)
            {
                // Loop over vertices in the face
                for (size_t v = 0; v < numVerts; v++)
                {
                    tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];

                    OBJVertex vertex{};

                    // Position
                    vertex.position.x = attrib.vertices[3 * static_cast<size_t>(idx.vertex_index) + 0];
                    vertex.position.y = attrib.vertices[3 * static_cast<size_t>(idx.vertex_index) + 1];
                    vertex.position.z = attrib.vertices[3 * static_cast<size_t>(idx.vertex_index) + 2];

                    // Normal
                    if (idx.normal_index >= 0)
                    {
                        vertex.normal.x = attrib.normals[3 * static_cast<size_t>(idx.normal_index) + 0];
                        vertex.normal.y = attrib.normals[3 * static_cast<size_t>(idx.normal_index) + 1];
                        vertex.normal.z = attrib.normals[3 * static_cast<size_t>(idx.normal_index) + 2];
                    }

                    // Texture coordinates
                    if (idx.texcoord_index >= 0)
                    {
                        vertex.uv.x = attrib.texcoords[(2 * static_cast<size_t>(idx.texcoord_index)) + 0];
                        vertex.uv.y = attrib.texcoords[(2 * static_cast<size_t>(idx.texcoord_index)) + 1];
                    }

                    // Create unique vertex key for deduplication
                    std::string vertexKey = std::format("{},{},{},{},{},{},{},{}",
                                                        vertex.position.x, vertex.position.y, vertex.position.z,
                                                        vertex.normal.x, vertex.normal.y, vertex.normal.z,
                                                        vertex.uv.x, vertex.uv.y);

                    // Check if vertex already exists
                    if (!uniqueVertices.contains(vertexKey))
                    {
                        uniqueVertices[vertexKey] = currentIndex;
                        vertices.push_back(vertex);
                        indices.push_back(currentIndex);
                        currentIndex++;
                    }
                    else
                    {
                        indices.push_back(uniqueVertices[vertexKey]);
                    }
                }
                indexOffset += numVerts;
            }
        }

        MeshAsset::MeshData meshData{};
        meshData.positions.reserve(vertices.size());
        meshData.normals.reserve(vertices.size());
        meshData.uvs.reserve(vertices.size());
        meshData.indices.reserve(indices.size());

        for (auto& m : vertices)
        {
            meshData.positions.push_back(m.position);
            meshData.normals.push_back(m.normal);
            meshData.uvs.push_back(m.uv);
        }

        for (auto& ind : indices)
        {
            meshData.indices.push_back(ind);
        }

        auto mesh = std::make_unique<MeshAsset>(meshData);

        for (const auto& mat : materials)
        {
            //auto material = std::make_unique<MaterialAsset>();
            /*material->addParameter("ambientColor", Vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]));
			material->addParameter("diffuseColor", Vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]));
			material->addParameter("specularColor", Vec3(mat.specular[0], mat.specular[1], mat.specular[2]));
			material->addParameter("shininess", mat.shininess);*/


            //uto matHandle = chai::AssetManager::instance().add(std::move(material));

            //if(matHandle.has_value())
            //	mesh->addMaterial(matHandle.value());
        }

        return mesh;
    }
}