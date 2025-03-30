#pragma once
#include <OpenGLRendererExport.h>
#include <ChaiEngine/Shader.h>
#include <ChaiEngine/UniformBuffer.h>
#include <set>

namespace chai::brew
{
	class OPENGLRENDERER_EXPORT GLShader : public Shader
	{
	public:
		GLShader();

		//the source here isnt the path, its the raw data
		void createShader(const char* source, brew::ShaderStage stage);
		void bind() override;
		bool isBound() override;

		int getHandle();
		ShaderStage getStage() { return m_stage; }

	private:
		bool m_isBound = false;
		int m_shaderHandle;
		ShaderStage m_stage;
	};
}