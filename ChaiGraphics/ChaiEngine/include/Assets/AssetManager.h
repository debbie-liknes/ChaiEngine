#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <Assets/MeshAsset.h>
#include <Assets/MaterialAsset.h>
#include <Resource/ResourceHandle.h>
#include <Resource/ResourceManager.h>
#include <filesystem>

namespace chai
{
    //using AssetId = uint64_t;

    //class AssetManager 
    //{
    //public:
    //    static AssetManager& instance();
    //    std::shared_ptr<MeshAsset> loadMesh(const std::string& path);
    //    std::shared_ptr<MaterialAsset> loadMaterial(const std::string& path);
    //    void preloadAssets(const std::vector<std::string>& assetList);

    //    AssetId requestMesh(const std::filesystem::path& path) 
    //    {
    //        AssetId id = hashPath(path);

    //        size_t key = std::hash<std::string>{}(path.string());

    //        //std::shared_ptr<IMesh> mesh;
    //        //if (!cache_.getMesh(key, cooked)) {
    //        //    // Import
    //        //    std::any raw = importAny(path);
    //        //    // Cook
    //        //    cooked = cookAnyToMesh(raw, id);
    //        //    cache_.putMesh(key, cooked);
    //        //}

    //        //return ResourceManager::instance().findOrCreateMesh<MeshHandle>(key);
    //        //return ResourceManager::instance().createAsync<Mesh>(cookedMesh);
    //        return 0;
    //    }
    //private:
    //    std::unordered_map<std::string, std::shared_ptr<MeshAsset>> m_meshes;
    //    std::unordered_map<std::string, std::shared_ptr<MaterialAsset>> m_materials;

    //    static AssetId hashPath(const std::filesystem::path& p) 
    //    {
    //        return static_cast<AssetId>(std::hash<std::string>{}(p.string()));
    //    }
    //};
}