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
        MeshAsset(std::shared_ptr<IMesh> mesh, std::shared_ptr<IMaterial> mat);

        static std::shared_ptr<MeshAsset> load(const std::string& path);
        IMesh* getMesh() const { return m_mesh.get(); }
		IMaterial* getMaterial() const { return m_material.get(); }

        bool isValid() const { return m_valid; }
        const std::string& getResourceId() const override { return m_assetId; }

    private:
        std::shared_ptr<IMesh> m_mesh;
        std::shared_ptr<IMaterial> m_material;
        std::string m_assetId;
        int m_refCount = 0;
        bool m_valid = false;
    };
}