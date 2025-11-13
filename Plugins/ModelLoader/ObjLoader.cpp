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

	std::shared_ptr<IAsset> ObjLoader::load(const std::string& path)
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

		auto mesh = std::make_shared<Mesh>(vertices, indices);

		for (auto& material : materials)
		{
			auto matHandle = chai::AssetManager::instance().load<chai::Material>(material.name);

			if(matHandle.has_value())
				mesh->addMaterial(matHandle.value());
		}

		return mesh;
	}

	bool MtlLoader::canLoad(const std::string& ext) const
	{
		return ext == "mtl";
	}

	std::shared_ptr<IAsset> MtlLoader::load(const std::string& path)
	{
		std::filesystem::path filePath(path);
		tinyobj::MaterialFileReader reader(filePath.parent_path().string());

		std::string fileName = filePath.filename().string();
		std::vector<tinyobj::material_t> materials;
		std::map<std::string, int> matMap;
		std::string warn;
		std::string err;

		bool success = reader(fileName, &materials, &matMap, &warn, &err);

		if (materials.empty())
		{
			std::cerr << "ERROR: could not load material: " << path << std::endl;
		}

		if (!warn.empty()) std::cerr << "WARN: " << warn << "\n";
		if (!err.empty()) std::cerr << "ERR: " << err << "\n";

		if (success) 
		{
			for (const auto& mat : materials) 
			{
				std::cout << "Loaded material: " << mat.name << "\n";
			}
		}

		//TODO: need to support multiple materials in 1 file
		auto const& tinyMat = materials[0];

		auto mat = MaterialSystem::createPhong();
		std::cout << "Applying material: " << tinyMat.name << std::endl;

		// Set specular color
		if (tinyMat.specular[0] != 0.0f || tinyMat.specular[1] != 0.0f || tinyMat.specular[2] != 0.0f)
		{
			mat->setSpecular(Vec3(tinyMat.specular[0], tinyMat.specular[1], tinyMat.specular[2]));
		}

		if (tinyMat.diffuse[0] != 0.0f || tinyMat.diffuse[1] != 0.0f || tinyMat.diffuse[2] != 0.0f)
		{
			mat->setDiffuse(Vec3(tinyMat.diffuse[0], tinyMat.diffuse[1], tinyMat.diffuse[2]));
		}

		// Set ambient color
		if (tinyMat.ambient[0] != 0.0f || tinyMat.ambient[1] != 0.0f || tinyMat.ambient[2] != 0.0f)
		{
			mat->setAmbient(Vec3(tinyMat.ambient[0], tinyMat.ambient[1], tinyMat.ambient[2]));
		}

		// Set shininess/specular exponent
		if (tinyMat.shininess > 0.0f)
		{
			mat->setShininess(tinyMat.shininess);
		}

		// Set transparency/alpha
		if (tinyMat.dissolve < 1.0f)
		{
			mat->setTransparency(tinyMat.dissolve);
		}

		return nullptr;
	}
}