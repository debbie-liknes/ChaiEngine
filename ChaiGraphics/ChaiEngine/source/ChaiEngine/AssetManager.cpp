#include <ChaiEngine/AssetManager.h>

namespace chai::brew
{
	std::shared_ptr<MeshAsset> AssetManager::loadMesh(const std::string& path)
	{
		auto it = meshes.find(path);
		if (it != meshes.end())
		{
			return it->second;
		}
		auto meshAsset = MeshAsset::load(path);
		if (meshAsset)
		{
			meshes[path] = meshAsset;
			return meshAsset;
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