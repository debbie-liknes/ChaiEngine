#include <Assets/MaterialAsset.h>
#include <Resource/ResourceManager.h>
#include <iostream>
#include <ChaiEngine/Material.h>

namespace chai::brew
{
	MaterialAsset::MaterialAsset(std::shared_ptr<IMaterial> mat) : m_material(mat)
	{
		m_material = brew::MaterialSystem::createPhong();
	}

	std::shared_ptr<MaterialAsset> MaterialAsset::load(const std::string& path)
	{
		if (auto asset = ResourceManager::instance().load<MaterialAsset>(path))
		{
			asset->m_assetId = path;
			asset->m_valid = true; // Mark as valid since we successfully loaded it
			return asset;
		}

		std::cout << "Error: Material " << path << "failed to load" << std::endl;
		return nullptr;
	}
}