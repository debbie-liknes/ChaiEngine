#include <OpenGLRenderer/GLShader.h>

namespace chai::brew
{
	GLShader::GLShader()
	{

	}

	void GLShader::bind()
	{
	}

	void GLShader::bind(int shader, ShaderStage stage)
	{
		m_isBound = true;
		m_shaderHandle = shader;
		m_stage = stage;
	}

	bool GLShader::isBound()
	{
		return m_isBound;
	}

	int GLShader::getHandle()
	{
		return m_shaderHandle;
	}

	GLShaderProgram::GLShaderProgram(int program)
	{
		m_programHandle = program;
	}

	void GLShaderProgram::AddShader(int handle)
	{
		m_shaders.insert(handle);
	}

	std::set<int> GLShaderProgram::getShaders()
	{
		return m_shaders;
	}
}