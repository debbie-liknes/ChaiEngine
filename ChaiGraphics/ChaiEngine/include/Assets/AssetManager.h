#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <Assets/MeshAsset.h>
#include <Assets/MaterialAsset.h>

namespace chai::brew
{
    class AssetManager 
    {
    public:
        static AssetManager& instance();
        std::shared_ptr<MeshAsset> loadMesh(const std::string& path);
        std::shared_ptr<MaterialAsset> loadMaterial(const std::string& path);
        void preloadAssets(const std::vector<std::string>& assetList);
    private:
        std::unordered_map<std::string, std::shared_ptr<MeshAsset>> m_meshes;
        std::unordered_map<std::string, std::shared_ptr<MaterialAsset>> m_materials;
    };
}