#pragma once
#include <OpenGLRendererExport.h>
#include <ChaiEngine/Shader.h>
#include <set>

namespace chai::brew
{
	class GLShaderProgram {
	public:
		GLShaderProgram(int program);
		void AddShader(int handle);
		std::set<int> getShaders();
		int getProgramHandle(){ return m_programHandle; }

	private:
		std::set<int> m_shaders;
		int m_programHandle;
	};

	class OPENGLRENDERER_EXPORT GLShader : public Shader
	{
	public:
		GLShader();
		void bind() override;
		void bind(int shader, ShaderStage stage);
		bool isBound() override;

		int getHandle();
		ShaderStage getStage() { return m_stage; }

	private:
		bool m_isBound = false;
		int m_shaderHandle;
		ShaderStage m_stage;
	};
}