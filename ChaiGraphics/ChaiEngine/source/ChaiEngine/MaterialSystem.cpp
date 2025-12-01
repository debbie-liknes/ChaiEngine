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
        loadPBRDefault();
        loadGBufferShader();
        loadLightingShader();
        loadSkyboxShader();
    }
}