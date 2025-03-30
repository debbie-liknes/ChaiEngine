#include <OpenGLRenderer/GLShaderProgram.h>
#include <glad/glad.h>
#include <stdexcept>
#include <iostream>

namespace chai::brew
{
	GLShaderProgram::GLShaderProgram()
	{
		unsigned int shaderProgram;
		shaderProgram = glCreateProgram();

		m_programHandle = shaderProgram;
	}

	void GLShaderProgram::link()
	{
		m_linked = true;
		glLinkProgram(m_programHandle);

		int  success;
		char infoLog[512];
		glGetProgramiv(m_programHandle, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(m_programHandle, 512, NULL, infoLog);
		}
	}

	void GLShaderProgram::use()
	{
		glUseProgram(m_programHandle);
	}

	void GLShaderProgram::AddShader(int handle)
	{
		m_shaders.insert(handle);
		glAttachShader(m_programHandle, handle);
	}

	std::set<int> GLShaderProgram::getShaders()
	{
		return m_shaders;
	}

	void GLShaderProgram::addUniform(std::string name, std::shared_ptr<UniformBufferBase> ubo, unsigned int binding)
	{

		unsigned int uniform;
		glGenBuffers(1, &uniform);
		glBindBuffer(GL_UNIFORM_BUFFER, uniform);
		glBufferData(GL_UNIFORM_BUFFER, ubo->getElementSize(), ubo->getRawData(), GL_STATIC_DRAW);

		GLuint blockIndex = glGetUniformBlockIndex(m_programHandle, ubo->name.c_str());
		glUniformBlockBinding(m_programHandle, blockIndex, binding);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, uniform);

		m_uniforms[name] = { ubo, uniform };
	}

	void GLShaderProgram::upload(std::string name)
	{
		UniformBindingPair pair = m_uniforms[name];
		glBindBuffer(GL_UNIFORM_BUFFER, pair.binding);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, pair.ubo->getElementSize(), pair.ubo->getRawData());
	}
}