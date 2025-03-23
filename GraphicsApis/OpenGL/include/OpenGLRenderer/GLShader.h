#pragma once
#include <OpenGLRendererExport.h>
#include <ChaiEngine/Shader.h>

namespace chai_graphics
{
	class OPENGLRENDERER_EXPORT GLShader : public Shader
	{
        // Generalized interface
        virtual void compile(const std::string& vertexSource, const std::string& fragmentSource) = 0;
        virtual void setUniform(const std::string& name, float value) = 0;
        virtual void setUniform(const std::string& name, const std::vector<float>& values) = 0;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        // Factory method
        static Shader* create(bool useVulkan);
	};
}