#pragma once
#include <Resource/ResourceLoader.h>
#include <Resource/ResourceHandle.h>
#include <ChaiEngine/IMaterial.h>

namespace chai
{
    //using MaterialHandle = ResourceHandle<struct MaterialTag>;
    class MaterialAsset : public IAsset
    {
    public:
        explicit MaterialAsset(std::shared_ptr<IMaterial> mat);

        static std::shared_ptr<MaterialAsset> load(const std::string& path);
        IMaterial* getMaterial() const { return m_material.get(); }

        bool isValid() const override { return m_valid; }
        const std::string& getAssetId() const override { return m_assetId; }

    private:
        std::shared_ptr<IMaterial> m_material;
        std::string m_assetId;
        int m_refCount = 0;
        bool m_valid = false;
    };
}