#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/Material.h>
#include <Resource/ResourceManager.h>
#include <Asset/AssetManager.h>

#include "Graphics/ShaderAsset.h"

namespace chai
{
    class CHAIGRAPHICS_EXPORT MaterialSystem
    {
    public:
        MaterialSystem()
        {
            loadPhongShaderDefault();
            createDefaultAsset();
        }

        ~MaterialSystem()
        {
        }

        static ResourceHandle createMaterialResourceFromAsset(const MaterialAsset* asset)
        {
            auto resource = std::make_unique<MaterialResource>();

            //set which shader to use
            resource->shaderAsset = asset->getShaderHandle();

            // Copy all parameters
            for (const auto& [name, value] : asset->getParameters())
            {
                resource->defaultParameters.emplace(name, value);
            }

            return ResourceManager::instance().add(std::move(resource));
        }

        static ResourceHandle createMaterialResourceFromAsset(AssetHandle asset)
        {
            return createMaterialResourceFromAsset(AssetManager::instance().get<MaterialAsset>(asset));
        }

        AssetHandle getDefaultAsset()
        {
            return m_defaultAsset;
        }

    private:
        void createDefaultAsset()
        {
            auto matAsset = std::make_unique<MaterialAsset>("phong_default", m_phongDefaultAsset);
            matAsset->setParameter("u_DiffuseColor", chai::Vec3(1.0, 1.0, 1.0));
            matAsset->setParameter("u_SpecularColor", chai::Vec3(1.0, 1.0, 1.0));
            matAsset->setParameter("u_Shininess", 1.f);

            m_defaultAsset = AssetManager::instance().add(std::move(matAsset)).value();
        }

        void loadPhongShaderDefault()
        {
            auto vertAssetHandle = chai::AssetManager::instance().load<chai::ShaderStageAsset>("shaders/phong.vert");
            auto vertAsset = chai::AssetManager::instance().get<chai::ShaderStageAsset>(vertAssetHandle.value());
            auto fragAssetHandle = chai::AssetManager::instance().load<chai::ShaderStageAsset>("shaders/phong.frag");
            auto fragAsset = chai::AssetManager::instance().get<chai::ShaderStageAsset>(fragAssetHandle.value());

            auto shaderAsset = std::make_unique<ShaderAsset>("phong_shader");
            shaderAsset->addStage(vertAsset->source);
            shaderAsset->addStage(fragAsset->source);
            shaderAsset->addVertexInput("a_Position", 0, chai::MaterialParameterType::Float3);
            shaderAsset->addVertexInput("a_Normal", 1, chai::MaterialParameterType::Float3);
            shaderAsset->addVertexInput("a_TexCoord", 2, chai::MaterialParameterType::Float2);

            m_phongDefaultAsset = AssetManager::instance().add(std::move(shaderAsset)).value();
        }

        AssetHandle m_defaultAsset;
        AssetHandle m_phongDefaultAsset;
    };
}