#pragma once
#include <ChaiEngine/Material.h>
#include <glad/gl.h>
#include <Types/CMap.h>
#include <iostream>
#include <set>
#include <sstream>


namespace chai::brew
{
    struct OpenGLShaderData
    {
        GLuint program = 0;
        AssetHandle shaderAssetHandle;

        // Material uniform locations
        CMap<std::string, GLint> uniformLocations;

        // UBO binding points
        GLuint perFrameUBOBinding = 0;
        GLuint perDrawUBOBinding = 1;
        GLuint lightingUBOBinding = 2;
    };

    class GLShaderManager
    {
    public:
        GLShaderManager() = default;
        virtual ~GLShaderManager();

        GLShaderManager(const GLShaderManager&) = delete;
        GLShaderManager& operator=(const GLShaderManager&) = delete;

        GLShaderManager(GLShaderManager&&) = default;
        GLShaderManager& operator=(GLShaderManager&&) = default;

        GLuint createDefaultShaderProgram();
        GLuint compileShaderFromAsset(AssetHandle shaderAssetHandle);

        GLuint compileShader(const char* source, GLenum type);
        GLuint compileShaderProgram(const char* vertexSource, const char* fragmentSource);

        OpenGLShaderData* getShaderData(GLuint program)
        {
            auto it = m_programToShaderData.find(program);
            if (it != m_programToShaderData.end())
            {
                return it->second.get();
            }
            return nullptr;
        }

    private:
        std::unordered_map<GLuint, std::unique_ptr<OpenGLShaderData>> m_programToShaderData;
        GLuint m_phongShaderProgram = 0;
    };
}