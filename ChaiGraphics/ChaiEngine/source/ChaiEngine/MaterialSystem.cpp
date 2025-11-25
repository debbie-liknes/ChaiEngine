#include <ChaiEngine/MaterialSystem.h>

#include <iostream>

namespace chai
{
    MaterialSystem& MaterialSystem::instance()
    {
        static MaterialSystem instance;
        return instance;
    }

    MaterialSystem::MaterialSystem()
    {
        loadPhongShaderDefault();
    }

    /*ResourceHandle MaterialSystem::createMaterialResourceFromAsset(AssetHandle matAsset)
    {
        auto asset = AssetManager::instance().get<MaterialAsset>(matAsset);
        if (!asset) {
            std::cerr << "Invalid material asset!" << std::endl;
            return ResourceHandle{};
        }

        // Get shader to validate against
        auto* shader = AssetManager::instance().get<ShaderAsset>(asset->getShaderHandle());
        if (!shader) {
            std::cerr << "Material references invalid shader!" << std::endl;
            return ResourceHandle{};
        }

        // ========================================================================
        // VALIDATION: Check material is compatible with shader
        // ========================================================================
        if (!validateMaterialAgainstShader(asset, shader)) {
            /*std::cerr << "Material '" << asset->getName()
                << "' is not compatible with shader '"
                << shader->getName() << "'" << std::endl;#1#
            return ResourceHandle{};
        }

        auto resource = std::make_unique<MaterialResource>();
        resource->sourceAsset = matAsset;
        resource->shaderAsset = asset->getShaderHandle();

        // Copy parameters (now validated!)
        for (const auto& [name, value] : asset->getParameters()) {
            resource->defaultParameters.emplace(name, value);
        }

        return ResourceManager::instance().add(std::move(resource));
    }

    bool MaterialSystem::validateMaterialAgainstShader(const MaterialAsset* material,
                                                       const ShaderAsset* shader)
    {
        bool valid = true;

        // Check all required shader uniforms are provided
        for (const auto& uniform : shader->getUniforms()) {
            if (!uniform.isRequired) {
                continue;
            }

            const auto* param = material->getParameter(uniform.name);
            if (!param) {
                std::cerr << "ERROR: Material missing required parameter: "
                    << uniform.name << std::endl;
                valid = false;
                continue;
            }

            // Check type compatibility
            if (!isTypeCompatible(*param, uniform.type)) {
                std::cerr << "ERROR: Parameter '" << uniform.name
                    << "' has wrong type!" << std::endl;
                valid = false;
            }
        }

        // Warn about unused parameters
        for (const auto& [name, value] : material->getParameters()) {
            bool used = false;
            for (const auto& uniform : shader->getUniforms()) {
                if (uniform.name == name) {
                    used = true;
                    break;
                }
            }

            if (!used) {
                std::cout << "WARNING: Material parameter '" << name
                    << "' not used by shader" << std::endl;
            }
        }

        return valid;
    }

    bool MaterialSystem::isTypeCompatible(const MaterialParameterValue& value, DataType expectedType)
    {
        return std::visit([expectedType](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, float>) {
                return expectedType == DataType::Float;
            }
            else if constexpr (std::is_same_v<T, Vec2>) {
                return expectedType == DataType::Float2;
            }
            else if constexpr (std::is_same_v<T, Vec3>) {
                return expectedType == DataType::Float3;
            }
            else if constexpr (std::is_same_v<T, Vec4>) {
                return expectedType == DataType::Float4;
            }
            else if constexpr (std::is_same_v<T, int>) {
                return expectedType == DataType::Int || expectedType == DataType::Bool;
            }
            else if constexpr (std::is_same_v<T, Mat3>) {
                return expectedType == DataType::Mat3;
            }
            else if constexpr (std::is_same_v<T, Mat4>) {
                return expectedType == DataType::Mat4;
            }
            else if constexpr (std::is_same_v<T, ResourceHandle>) {
                return expectedType == DataType::Sampler2D ||
                       expectedType == DataType::SamplerCube;
            }
            else if constexpr (std::is_same_v<T, bool>) {
                return expectedType == DataType::Bool;
            }
            return false;
        }, value);
    }*/
}