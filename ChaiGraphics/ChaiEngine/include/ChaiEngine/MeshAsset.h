#pragma once
#include <Resource/ResourceLoader.h>
#include <ChaiEngine/Vertex.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>

namespace chai::brew
{
    class MeshAsset : public IResource
    {
    public:
        MeshAsset(std::shared_ptr<IMesh> mesh);

        static std::shared_ptr<MeshAsset> load(const std::string& path);
        std::shared_ptr<brew::IMesh> getMesh() const { return m_mesh; }
        std::vector<std::string> getMaterials() const { return m_materialLibraries; }

        void addMaterialLibrary(const std::string& mat) { m_materialLibraries.push_back(mat); }

        bool isValid() const override { return m_valid; }
        const std::string& getResourceId() const override { return m_assetId; }

    private:
        std::shared_ptr<IMesh> m_mesh;
        std::string m_assetId;
        int m_refCount = 0;
        bool m_valid = false;
        std::vector<std::string> m_materialLibraries;
    };
}