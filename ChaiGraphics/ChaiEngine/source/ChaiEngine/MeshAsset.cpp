#include <ChaiEngine/MeshAsset.h>
#include <Resource/ResourceManager.h>
#include <iostream>

namespace chai::brew
{
	MeshAsset::MeshAsset(std::shared_ptr<IMesh> mesh) : m_mesh(mesh), m_valid(false)
	{
	}

	std::shared_ptr<MeshAsset> MeshAsset::load(const std::string& path)
	{
		if (auto asset = ResourceManager::instance().load<MeshAsset>(path))
		{
			asset->m_assetId = path;
			asset->m_valid = true; // Mark as valid since we successfully loaded it
			return asset;
		}

		std::cout << "Error: Mesh " << path << "failed to load" << std::endl;
		return nullptr;
	}
}