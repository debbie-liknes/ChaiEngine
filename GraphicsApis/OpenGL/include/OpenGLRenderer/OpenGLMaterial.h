#pragma once
#include <ChaiEngine/IMaterial.h>
#include <glad/gl.h>
#include <vector>
#include <unordered_map>

namespace chai::brew
{
    struct OpenGLMaterialData 
    {
        GLuint shaderProgram = 0;
        bool isCompiled = false;

        std::unordered_map<std::string, GLint> uniformLocations;

        std::vector<int> boundTextures;

        uint32_t lastAppliedVersion = 0;
    };
}