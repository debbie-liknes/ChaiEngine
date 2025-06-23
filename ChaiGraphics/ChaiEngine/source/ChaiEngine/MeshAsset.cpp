#include <ChaiEngine/IMesh.h>
#include <Resource/ResourceManager.h>

namespace chai::brew
{
	MeshAsset::MeshAsset(std::shared_ptr<IMesh> mesh) : m_mesh(mesh)
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
		//log and error if mesh loading fails

		return nullptr;
	}
}