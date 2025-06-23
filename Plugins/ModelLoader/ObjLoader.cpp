#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "ObjLoader.h"
#include <iostream>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/Vertex.h>

namespace chai
{
	ObjLoader::ObjLoader()
	{
	}

	bool ObjLoader::canLoad(const std::string& ext) const
	{
		return ext == "obj";
	}

    std::shared_ptr<IResource> ObjLoader::load(const std::string& path)
    {
        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = "./";
        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(path, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            return nullptr; // Don't exit, return null
        }

        if (!reader.Warning().empty()) {
            std::cout << "TinyObjReader: " << reader.Warning();
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        auto& materials = reader.GetMaterials();

        // Create mesh data containers
        std::vector<brew::Vertex> vertices;
        std::vector<uint32_t> indices;
        std::unordered_map<std::string, uint32_t> uniqueVertices;

        uint32_t currentIndex = 0;

        // Loop over shapes
        for (size_t s = 0; s < shapes.size(); s++) {
            size_t index_offset = 0;

            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

                // Loop over vertices in the face
                for (size_t v = 0; v < fv; v++) {
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                    brew::Vertex vertex{};

                    // Position
                    vertex.position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    vertex.position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    vertex.position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    // Normal
                    if (idx.normal_index >= 0) {
                        vertex.normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        vertex.normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        vertex.normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
                    }

                    // Texture coordinates
                    if (idx.texcoord_index >= 0) {
                        vertex.texCoord.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        vertex.texCoord.y = 1.0f - attrib.texcoords[2 * size_t(idx.texcoord_index) + 1]; // Flip Y
                    }

                    // Create unique vertex key for deduplication
                    std::string vertexKey = std::to_string(vertex.position.x) + "," +
                        std::to_string(vertex.position.y) + "," +
                        std::to_string(vertex.position.z) + "," +
                        std::to_string(vertex.normal.x) + "," +
                        std::to_string(vertex.normal.y) + "," +
                        std::to_string(vertex.normal.z) + "," +
                        std::to_string(vertex.texCoord.x) + "," +
                        std::to_string(vertex.texCoord.y);

                    // Check if vertex already exists
                    if (uniqueVertices.count(vertexKey) == 0) {
                        uniqueVertices[vertexKey] = currentIndex;
                        vertices.push_back(vertex);
                        indices.push_back(currentIndex);
                        currentIndex++;
                    }
                    else {
                        indices.push_back(uniqueVertices[vertexKey]);
                    }
                }
                index_offset += fv;
            }
        }

        // Create your IMesh implementation
        auto mesh = std::make_shared<brew::Mesh>(vertices, indices);

        // Create and return MeshAsset
        return std::make_shared<brew::MeshAsset>(mesh);
    }
} 