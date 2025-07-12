#include <OpenGLRenderer/GLShader.h>
#include <glad/gl.h>
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>

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

	//GLShaderLoader::GLShaderLoader()
	//{

	//}

	//GLShaderLoader::GLShaderLoader(Renderer* renderer) : m_renderer(renderer) {}

	//bool GLShaderLoader::CanLoad(const std::string& ext) const
	//{
	//	return ext == "vert" ||
	//		   ext == "frag";
	//}

	//std::shared_ptr<IResource> GLShaderLoader::Load(const std::string& path)
	//{
	//	if (!m_renderer) return nullptr;

	//	auto shaderResource = std::make_shared<ShaderResource>();
	//	std::string fullpath = std::string(CMAKE_SOURCE_DIR) + "/" + path;
	//	std::ifstream shaderFile(fullpath);
	//	std::stringstream buffer;
	//	buffer << shaderFile.rdbuf();
	//	auto shader = std::make_shared<GLShader>();
	//	shader->shaderSource = buffer.str();
	//	shaderResource->shader = shader;
	//	return shaderResource;
	//}
}