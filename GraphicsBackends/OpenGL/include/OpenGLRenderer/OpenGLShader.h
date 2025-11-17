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

        // Material uniform locations
        //std::unordered_map<std::string, GLint> uniformLocations;

        // UBO binding points
        GLuint perFrameUBOBinding = 0;
        GLuint perDrawUBOBinding = 1;
        GLuint lightingUBOBinding = 2;
        ////////////////////////////////////////////////////////

        // Cached built-in uniform locations
        //GLint u_transform = -1;
        //GLint u_lightCount = -1;

        //// Cache light uniform locations (for 16 lights)
        //struct LightUniforms 
        //{
        //    GLint type = -1;
        //    GLint position = -1;
        //    GLint direction = -1;
        //    GLint color = -1;
        //    GLint intensity = -1;
        //    GLint range = -1;
        //    GLint attenuation = -1;
        //    GLint innerCone = -1;
        //    GLint outerCone = -1;
        //    GLint enabled = -1;
        //};
        //std::array<LightUniforms, 16> lights;
	};

	class GLShaderManager
	{
	public:
		//static std::string generateShaderHash(std::shared_ptr<ShaderDescription> shaderDesc,
		//	const std::set<MaterialFeature>& features);

		GLuint createDefaultShaderProgram();
		GLuint compileShader(const char* source, GLenum type);

        OpenGLShaderData* getShaderData(GLuint program)
        {
            auto it = m_programToShaderData.find(program);
            if (it != m_programToShaderData.end())
            {
                return it->second;
            }
            return nullptr;
		}

    private:
		CMap<GLuint, OpenGLShaderData*> m_programToShaderData;
	};
}