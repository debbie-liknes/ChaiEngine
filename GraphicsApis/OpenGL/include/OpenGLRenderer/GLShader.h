#pragma once
#include <OpenGLRendererExport.h>
#include <ChaiEngine/Shader.h>

namespace chai::brew
{
	class OPENGLRENDERER_EXPORT GLShader : public Shader
	{
	public:
		void Bind();
	};
}