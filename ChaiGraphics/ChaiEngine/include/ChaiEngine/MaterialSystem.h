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

        //      static Handle getPhongHandle()
        //      {
        //          static std::optional<Handle> phongHandle;
              //	if (phongHandle.has_value())
        //              return phongHandle.value();

              //	phongHandle = chai::AssetManager::instance().add(createPhong());
              //	return phongHandle.value();
              //}

              //// PBR materials
              //static std::shared_ptr<Material> createPBR() 
              //{
              //    auto shaderDesc = std::make_shared<ShaderDescription>();
              //    shaderDesc->name = "pbr_standard";
              //    shaderDesc->stages = {
              //        {ShaderStage::Vertex, "shaders/pbr.vert"},
              //        {ShaderStage::Fragment, "shaders/pbr.frag"}
              //    };

              //    auto material = std::make_shared<Material>(shaderDesc);

              //    // Set default PBR values
              //    material->setBaseColor(Vec3(0.8f, 0.8f, 0.8f));
              //    material->setMetallic(0.0f);
              //    material->setRoughness(0.8f);

              //    return material;
              //}

              //// Legacy Phong for OBJ compatibility
              //static std::unique_ptr<Material> createPhong() 
              //{
              //    auto shaderDesc = std::make_shared<ShaderDescription>();
              //    shaderDesc->name = "phong_standard";
              //    shaderDesc->stages = {
              //        {.type = ShaderStage::Vertex, .path = "shaders/phong.vert"},
              //        {.type = ShaderStage::Fragment, .path = "shaders/phong.frag"}
              //    };

              //    auto material = std::make_unique<Material>(shaderDesc);

              //    // Set default Phong values
              //    material->setDiffuse(Vec3(0.8f, 0.8f, 0.8f));
              //    material->setSpecular(Vec3(0.2f, 0.2f, 0.2f));
              //    material->setAmbient(Vec3(0.1f, 0.1f, 0.1f));
              //    material->setShininess(32.0f);

              //    return std::move(material);
              //}

              //// Factory method that creates appropriate material based on features
              //static std::shared_ptr<Material> createFromFeatures(const std::set<MaterialFeature>& features) 
              //{
              //    bool needsPBR = features.count(MaterialFeature::Metallic) ||
              //        features.count(MaterialFeature::MetallicTexture) ||
              //        features.count(MaterialFeature::Roughness) ||
              //        features.count(MaterialFeature::RoughnessTexture);

              //    auto material = needsPBR ? createPBR() : createPhong();

              //    for (auto feature : features) {
              //        material->setFeature(feature, true);
              //    }

              //    return material;
              //}

              //static std::shared_ptr<Material> createCustom(const std::string& vertexPath,
              //    const std::string& fragmentPath) 
              //{
              //    auto shaderDesc = std::make_shared<ShaderDescription>();
              //    shaderDesc->name = "custom_" + std::to_string(generateUniqueId());
              //    shaderDesc->stages = {
              //        {ShaderStage::Vertex, vertexPath},
              //        {ShaderStage::Fragment, fragmentPath}
              //    };
              //    return std::make_shared<Material>(shaderDesc);
              //}

    private:
		MaterialSystem() = default;
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
        //static int generateUniqueId()
        //{
        //    static int idCounter = 0;
        //    return idCounter++;
        //}
    };
}