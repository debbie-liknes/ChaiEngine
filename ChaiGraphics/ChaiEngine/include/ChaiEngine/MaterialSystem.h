#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/Material.h>
#include <Resource/ResourceManager.h>
#include <Asset/AssetManager.h>

namespace chai
{
    class CHAIGRAPHICS_EXPORT MaterialSystem
    {
    public:
        static MaterialSystem& instance();

        Handle getDefaultMaterialAsset() const 
        {
            return m_defaultMaterialAsset;
        }

        std::unique_ptr<MaterialInstance> createInstance(Handle materialAssetHandle) {
            return std::make_unique<MaterialInstance>(materialAssetHandle);
        }

    private:
        MaterialSystem()
        {
            init();
        }
        void init();
        Handle m_defaultMaterialAsset;

        void createDefaultMaterial()
        {
            auto asset = std::make_unique<MaterialAsset>();
            //asset->name = "default_phong";
            asset->shaderName = "phong_standard";
            asset->properties.diffuseColor = Vec3(0.8f, 0.8f, 0.8f);
            asset->properties.specularColor = Vec3(0.2f, 0.2f, 0.2f);
            asset->properties.ambientColor = Vec3(0.1f, 0.1f, 0.1f);
            asset->properties.shininess = 32.0f;

            m_defaultMaterialAsset = AssetManager::instance().add(std::move(asset)).value();
        }
    };
}