#include <OpenGLRenderer/GLShader.h>
#include <glad/glad.h>
#include <iostream>

namespace chai::brew
{
	GLShader::GLShader()
	{

	}

	void GLShader::bind()
	{
	}

	int convertShaderType(ShaderStage stage)
	{
		switch (stage)
		{
		case brew::VERTEX:
			return GL_VERTEX_SHADER;
		case brew::FRAGMENT:
			return GL_FRAGMENT_SHADER;
		default:
			break;
		}
		return -1;
	}

	void GLShader::createShader(const char* source, brew::ShaderStage stage)
	{
		unsigned int shader;
		shader = glCreateShader(convertShaderType(stage));
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);

		int  success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		shaderSource = source;

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

	void GLShaderProgram::addUniform(std::string name, std::shared_ptr<UniformBufferBase> ubo)
	{
		m_uniforms[name] = ubo;
	}
}