#include <Assets/AssetManager.h>
#include <Assets/MaterialAsset.h>

namespace chai::brew
{
	std::shared_ptr<MeshAsset> AssetManager::loadMesh(const std::string& path)
	{
		if (auto meshItr = m_meshes.find(path); meshItr != m_meshes.end())
		{
			return meshItr->second;
		}

		if (auto meshAsset = MeshAsset::load(path); meshAsset)
		{
			m_meshes[path] = meshAsset;

			for (auto const& mat : meshAsset->getMaterials())
			{
				loadMaterial(path);
				MaterialAsset::load(mat);
			}
			return meshAsset;
		}
		
		return nullptr; // or throw an exception
	}

	std::shared_ptr<MaterialAsset> AssetManager::loadMaterial(const std::string& path)
	{
		if (auto materialItr = m_materials.find(path); materialItr != m_materials.end())
		{
			return materialItr->second;
		}

		if (auto materialAsset = MaterialAsset::load(path); materialAsset)
		{
			m_materials[path] = materialAsset;

			return materialAsset;
		}

		return nullptr; // or throw an exception
	}

	void AssetManager::preloadAssets(const std::vector<std::string>& assetList)
	{
		for (const auto& assetPath : assetList)
		{
			loadMesh(assetPath);
		}
	}

	AssetManager& AssetManager::instance()
	{
		static AssetManager instance;
		return instance;
	}
}