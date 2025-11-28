#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/Material.h>
#include <Resource/ResourceManager.h>
#include <Asset/AssetManager.h>

#include "Graphics/ShaderAsset.h"

#include <iostream>

namespace chai
{
    class CHAIGRAPHICS_EXPORT MaterialSystem
    {
    public:
        static MaterialSystem& instance();

        MaterialSystem(const MaterialSystem&) = delete;
        MaterialSystem& operator=(const MaterialSystem&) = delete;

        static ResourceHandle createFromAsset(AssetHandle assetHandle)
        {
            auto* asset = AssetManager::instance().get<MaterialAsset>(assetHandle);
            if (!asset) {
                std::cerr << "Invalid material asset!" << std::endl;
                return ResourceHandle{};
            }

            return createFromAsset(asset);
        }

        static ResourceHandle createFromAsset(const MaterialAsset* asset)
        {
            if (!asset)
                return ResourceHandle{};

            // Get shader for validation
            auto* shader = AssetManager::instance().get<ShaderAsset>(asset->getShaderHandle());
            if (!shader) {
                std::cerr << "Material references invalid shader!" << std::endl;
                return ResourceHandle{};
            }

            // Validate
            if (!validateParameters(asset->getParameters(), shader)) {
                std::cerr << "Material '" << asset->getName()
                    << "' is not compatible with shader!" << std::endl;
                return ResourceHandle{};
            }

            // Create resource
            auto resource = std::make_unique<MaterialResource>();
            resource->shaderAsset = asset->getShaderHandle();
            resource->defaultParameters = asset->getParameters();

            return ResourceManager::instance().add(std::move(resource));
        }

        static ResourceHandle create(AssetHandle shaderHandle)
        {
            auto* shader = AssetManager::instance().get<ShaderAsset>(shaderHandle);
            if (!shader) {
                std::cerr << "Invalid shader handle!" << std::endl;
                return ResourceHandle{};
            }

            auto resource = std::make_unique<MaterialResource>();
            resource->shaderAsset = shaderHandle;

            // Initialize with shader defaults
            for (const auto& uniform : shader->getUniforms()) {
                if (uniform.defaultValue.index() != 0) {
                    resource->defaultParameters[uniform.name] = uniform.defaultValue;
                }
            }

            return ResourceManager::instance().add(std::move(resource));
        }

        class Builder
        {
        public:
            explicit Builder(AssetHandle shaderHandle)
                : m_shaderHandle(shaderHandle)
            {
                m_shader = AssetManager::instance().get<ShaderAsset>(shaderHandle);
                if (!m_shader) {
                    std::cerr << "Invalid shader handle!" << std::endl;
                }
            }

            Builder& set(const std::string& name, const MaterialParameterValue& value)
            {
                if (!m_shader)
                    return *this;

                // Validate parameter exists in shader
                bool found = false;
                for (const auto& uniform : m_shader->getUniforms()) {
                    if (uniform.name == name) {
                        found = true;
                        // Could also validate type here
                        break;
                    }
                }

                if (!found) {
                    std::cerr << "WARNING: Parameter '" << name
                        << "' not used by shader" << std::endl;
                }

                m_parameters[name] = value;
                return *this;
            }

            // Convenience overloads
            Builder& setFloat(const std::string& name, float value)
            {
                return set(name, value);
            }

            Builder& setVec3(const std::string& name, const Vec3& value)
            {
                return set(name, value);
            }

            Builder& setTexture(const std::string& name, ResourceHandle texHandle)
            {
                return set(name, texHandle);
            }

            ResourceHandle build()
            {
                if (!m_shader) {
                    return ResourceHandle{};
                }

                // Validate all required parameters are provided
                if (!MaterialSystem::validateParameters(m_parameters, m_shader)) {
                    std::cerr << "Material missing required parameters!" << std::endl;
                    return ResourceHandle{};
                }

                auto resource = std::make_unique<MaterialResource>();
                resource->shaderAsset = m_shaderHandle;
                resource->defaultParameters = m_parameters;

                return ResourceManager::instance().add(std::move(resource));
            }

        private:
            AssetHandle m_shaderHandle;
            const ShaderAsset* m_shader;
            std::unordered_map<std::string, MaterialParameterValue> m_parameters;
        };

        /*static ResourceHandle createMaterialResourceFromAsset(AssetHandle handle);*/

        AssetHandle getPhongShader()
        {
            return m_phongShader;
        }

        AssetHandle getPBRShader() { return m_pbrShader; }

        AssetHandle getPhongMaterial()
        {
            return m_phongMaterial;
        }

    private:
        MaterialSystem();

        static bool validateParameters(
            const std::unordered_map<std::string, MaterialParameterValue>& params,
            const ShaderAsset* shader)
        {
            // Check all required uniforms are provided
            for (const auto& uniform : shader->getUniforms()) {
                if (!uniform.isRequired)
                    continue;

                auto it = params.find(uniform.name);
                if (it == params.end()) {
                    std::cerr << "ERROR: Missing required parameter: "
                        << uniform.name << std::endl;
                    return false;
                }
            }
            return true;
        }

        void loadPhongShaderDefault()
        {
            auto vertAssetHandle = AssetManager::instance().load<ShaderStageAsset>("shaders/phong.vert").value();
            auto fragAssetHandle = AssetManager::instance().load<ShaderStageAsset>("shaders/phong.frag").value();

            // Get the loaded stage assets
            auto vertAsset = AssetManager::instance().get<ShaderStageAsset>(vertAssetHandle);
            auto fragAsset = AssetManager::instance().get<ShaderStageAsset>(fragAssetHandle);

            auto shaderAsset = std::make_unique<ShaderAsset>("phong_shader");
            shaderAsset->addStage(*vertAsset);
            shaderAsset->addStage(*fragAsset);

            shaderAsset->addVertexInput("a_Position", 0, DataType::Float3);
            shaderAsset->addVertexInput("a_Normal", 1, DataType::Float3);
            shaderAsset->addVertexInput("a_TexCoord", 2, DataType::Float2);

            shaderAsset->addUniform("u_DiffuseColor", DataType::Float3);
            shaderAsset->addUniform("u_SpecularColor", DataType::Float3);
            shaderAsset->addUniform("u_Shininess", DataType::Float);
            shaderAsset->addUniform("u_DiffuseMap", DataType::Sampler2D);

            m_phongShader = AssetManager::instance().add(std::move(shaderAsset)).value();
        }

        void loadPBRDefault()
        {
            auto vertAssetHandle =
                AssetManager::instance().load<ShaderStageAsset>("shaders/pbr.vert").value();
            auto fragAssetHandle =
                AssetManager::instance().load<ShaderStageAsset>("shaders/pbr.frag").value();

            // Get the loaded stage assets
            auto vertAsset = AssetManager::instance().get<ShaderStageAsset>(vertAssetHandle);
            auto fragAsset = AssetManager::instance().get<ShaderStageAsset>(fragAssetHandle);

            auto shaderAsset = std::make_unique<ShaderAsset>("pbr_shader");
            shaderAsset->addStage(*vertAsset);
            shaderAsset->addStage(*fragAsset);

            shaderAsset->addVertexInput("a_Position", 0, DataType::Float3);
            shaderAsset->addVertexInput("a_Normal", 1, DataType::Float3);
            shaderAsset->addVertexInput("a_TexCoord", 2, DataType::Float2);

            shaderAsset->addUniform("u_albedo", DataType::Float3);
            shaderAsset->addUniform("u_albedoMap", DataType::Sampler2D);
            shaderAsset->addUniform("u_metallic", DataType::Float);
            shaderAsset->addUniform("u_metallicMap", DataType::Sampler2D);
            shaderAsset->addUniform("u_roughness", DataType::Float);
            shaderAsset->addUniform("u_roughnessMap", DataType::Sampler2D);

            m_pbrShader = AssetManager::instance().add(std::move(shaderAsset)).value();
        }

        AssetHandle m_phongShader;
        AssetHandle m_pbrShader;
        AssetHandle m_phongMaterial;
    };
}