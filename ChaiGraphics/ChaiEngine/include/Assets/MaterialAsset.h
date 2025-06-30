#pragma once
#include <Resource/ResourceLoader.h>
#include <ChaiEngine/IMaterial.h>

namespace chai::brew
{
    class MaterialAsset : public IResource
    {
    public:
        explicit MaterialAsset(std::shared_ptr<IMaterial> mat);

        static std::shared_ptr<MaterialAsset> load(const std::string& path);
        IMaterial* getMaterial() const { return m_material.get(); }

        bool isValid() const override { return m_valid; }
        const std::string& getResourceId() const override { return m_assetId; }

    private:
        std::shared_ptr<IMaterial> m_material;
        std::string m_assetId;
        int m_refCount = 0;
        bool m_valid = false;
    };
}