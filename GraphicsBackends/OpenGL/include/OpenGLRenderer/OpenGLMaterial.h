#pragma once
#include <ChaiEngine/Material.h>
#include <glad/gl.h>
#include <vector>
#include <Types/CMap.h>
#include <set>
#include <sstream>

namespace chai::brew
{
    struct OpenGLMaterialData 
    {
        GLuint shaderProgram = 0;
        bool isCompiled = false;

        CMap<std::string, GLint> uniformLocations;
        std::vector<int> boundTextures;
        uint32_t lastAppliedVersion = 0;
    };

    struct OpenGLShaderData
    {
        GLuint program = 0;

        // Cached built-in uniform locations
        GLint u_transform = -1;
        GLint u_view = -1;
        GLint u_projection = -1;
        GLint u_lightCount = -1;

        // Cache light uniform locations (for 16 lights)
        struct LightUniforms {
            GLint type = -1;
            GLint position = -1;
            GLint direction = -1;
            GLint color = -1;
            GLint intensity = -1;
            GLint range = -1;
            GLint attenuation = -1;
            GLint innerCone = -1;
            GLint outerCone = -1;
            GLint enabled = -1;
        };
        std::array<LightUniforms, 16> lights;
    };

    std::string generateShaderHash(std::shared_ptr<ShaderDescription> shaderDesc,
        const std::set<MaterialFeature>& features)
    {
        std::stringstream hash;

        // Include shader name/path
        hash << shaderDesc->name;

        // Include all shader stage paths (vertex, fragment, etc.)
        for (const auto& stage : shaderDesc->stages)
        {
            hash << "_" << stage.path;
        }

        // Include feature flags that affect compilation
        if (features.contains(MaterialFeature::BaseColorTexture))
            hash << "_BC";
        if (features.contains(MaterialFeature::NormalTexture))
            hash << "_NM";
        if (features.contains(MaterialFeature::MetallicTexture))
            hash << "_MT";
        if (features.contains(MaterialFeature::RoughnessTexture))
            hash << "_RG";
        if (features.contains(MaterialFeature::EmissionTexture))
            hash << "_EM";
        if (features.contains(MaterialFeature::Transparency))
            hash << "_TR";
        if (features.contains(MaterialFeature::DoubleSided))
            hash << "_DS";

        return hash.str();
    }
}