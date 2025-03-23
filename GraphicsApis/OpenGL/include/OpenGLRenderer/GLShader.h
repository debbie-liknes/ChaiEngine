#pragma once
#include <OpenGLRendererExport.h>
#include <ChaiEngine/Shader.h>

namespace chai_graphics
{
	class OPENGLRENDERER_EXPORT GLShader : public Shader
	{
        // Generalized interface
        void compile(const std::string& vertexSource, const std::string& fragmentSource);
        void setUniform(const std::string& name, float value);
        void setUniform(const std::string& name, const std::vector<float>& values);

        void bind() const = 0;
        void unbind() const = 0;
	};
}