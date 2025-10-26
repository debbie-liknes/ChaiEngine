#pragma once
#include <Resource/ResourceLoader.h>
#include <Resource/ResourceHandle.h>
#include <ChaiEngine/Vertex.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>

namespace chai
{
    //struct MeshTag {};
    //using MeshHandle = ResourceHandle<MeshTag>;


    //struct MeshView 
    //{
    //    uint32_t submeshCount = 0;
    //    float boundsMin[3]{};
    //    float boundsMax[3]{};
    //};

    //struct IMeshResource : IResource 
    //{
    //    virtual const MeshView& view()  const = 0;
    //    virtual ResourceState state() const = 0;
    //};

    //template<> struct ResourceTraits<IMeshResource>
    //{
    //    using Interface = IMeshResource;
    //    using Handle = MeshHandle;
    //    using Desc = IMesh;
    //    using CreatorFn = std::function<std::unique_ptr<IMeshResource>(const IMesh&)>;
    //    static constexpr const char* name = "Mesh";
    //};

    //class MeshAsset : public IAsset
    //{
    //public:
    //    MeshAsset(std::shared_ptr<IMesh> mesh);

    //    static std::shared_ptr<MeshAsset> load(const std::string& path);
    //    std::shared_ptr<IMesh> getMesh() const { return m_mesh; }
    //    std::vector<std::string> getMaterials() const { return m_materialLibraries; }

    //    void addMaterialLibrary(const std::string& mat) { m_materialLibraries.push_back(mat); }

    //    bool isValid() const override { return m_valid; }
    //    const std::string& getAssetId() const override { return m_assetId; }

    //private:
    //    std::shared_ptr<IMesh> m_mesh;
    //    std::string m_assetId;
    //    int m_refCount = 0;
    //    bool m_valid = false;
    //    std::vector<std::string> m_materialLibraries;
    //};
}