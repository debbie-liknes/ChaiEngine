#pragma once
#include <ChaiEngine/IMaterial.h>
#include <glad/gl.h>
#include <vector>
#include <Types/CMap.h>

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
}