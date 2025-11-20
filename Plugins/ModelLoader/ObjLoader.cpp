#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "ObjLoader.h"
#include <iostream>
#include <format>
#include <ChaiEngine/Vertex.h>
#include <ChaiEngine/Material.h>
#include <ChaiEngine/UniformBuffer.h>
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
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		CMap<std::string, uint32_t> uniqueVertices;

		uint32_t currentIndex = 0;

		// Loop over shapes
		for (auto& shape : shapes)
		{
			size_t index_offset = 0;
			for (auto& numVerts : shape.mesh.num_face_vertices)
			{
				// Loop over vertices in the face
				for (size_t v = 0; v < numVerts; v++)
				{
					tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

					Vertex vertex{};

					// Position
					vertex.position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
					vertex.position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
					vertex.position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

					// Normal
					if (idx.normal_index >= 0)
					{
						vertex.normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
						vertex.normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
						vertex.normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
					}

					// Texture coordinates
					if (idx.texcoord_index >= 0)
					{
						vertex.texCoord.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
						vertex.texCoord.y = 1.0f - attrib.texcoords[2 * size_t(idx.texcoord_index) + 1]; // Flip Y
					}

					// Create unique vertex key for deduplication
					std::string vertexKey = std::format("{},{},{},{},{},{},{},{}",
						vertex.position.x, vertex.position.y, vertex.position.z,
						vertex.normal.x, vertex.normal.y, vertex.normal.z,
						vertex.texCoord.x, vertex.texCoord.y);

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
				index_offset += numVerts;
			}
		}

		auto mesh = std::make_unique<MeshAsset>(vertices, indices);

		for (auto& mat : materials)
		{
			auto material = std::make_unique<MaterialAsset>();
			material->properties.ambientColor = Vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
			material->properties.diffuseColor = Vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
			material->properties.specularColor = Vec3(mat.specular[0], mat.specular[1], mat.specular[2]);
			material->properties.shininess = mat.shininess;
			material->texturePaths.diffuseMap = mat.diffuse_texname;


			auto matHandle = chai::AssetManager::instance().add(std::move(material));

			if(matHandle.has_value())
				mesh->addMaterial(matHandle.value());
		}

		return mesh;
	}
}