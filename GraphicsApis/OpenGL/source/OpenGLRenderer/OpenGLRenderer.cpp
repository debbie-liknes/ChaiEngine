#include <OpenGLRenderer/OpenGLRenderer.h>
#include <glad/glad.h>
#include <iostream>
#include <Engine/Viewport.h>
#include <Engine/Window.h>
#include <RenderObjects/RenderObject.h>

namespace CGraphics
{
	OpenGLBackend::OpenGLBackend()
	{
	}

	void OpenGLBackend::setProcAddress(void* address)
	{
		//call init on the graphics api instead of doing this here
		if (!gladLoadGLLoader((GLADloadproc)address))
		{
			std::cerr << "Failed to initialize GLAD" << std::endl;
		}
	}

	int convertShaderType(CGraphics::ShaderStage stage)
	{
		switch (stage)
		{
		case CGraphics::VERTEX:
			return GL_VERTEX_SHADER;
		case CGraphics::FRAGMENT:
			return GL_FRAGMENT_SHADER;
		default:
			break;
		}
		return -1;
	}

	int OpenGLBackend::createShader(const char* source, CGraphics::ShaderStage stage)
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

		return shader;
	}

	int OpenGLBackend::createShaderProgram(Core::CVector<int> shaders)
	{
		unsigned int shaderProgram;
		shaderProgram = glCreateProgram();

		for (auto& shader : shaders)
		{
			glAttachShader(shaderProgram, shader);
		}
		glLinkProgram(shaderProgram);

		int  success;
		char infoLog[512];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		}

		glUseProgram(shaderProgram);
		return shaderProgram;
	}

	void OpenGLBackend::renderFrame(Window* window, Core::CVector<Core::CSharedPtr<RenderObject>> ros)
	{
		for (auto& v : window->GetViewports())
		{
			int x, y, width, height;
			v->GetDimensions(x, y, width, height);
			glViewport(x, y, width, height);
		}

		//renders
		//this is not efficient, atm
		for (auto& ro : ros)
		{
			unsigned int VBO;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ro->m_verts), ro->m_verts.data(), GL_STATIC_DRAW);

			Core::CVector<int> shaders;
			for (auto& s : ro->m_data)
			{
				shaders.push_back(createShader(s.shaderSource.data(), s.stage));
			}

			int program = createShaderProgram(shaders);

			{
				//linking stuff
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);

				//VAO
				unsigned int VAO;
				glGenVertexArrays(1, &VAO);
				// ..:: Initialization code (done once (unless your object frequently changes)) :: ..
				// 1. bind Vertex Array Object
				glBindVertexArray(VAO);
				// 2. copy our vertices array in a buffer for OpenGL to use
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(ro->m_verts), ro->m_verts.data(), GL_STATIC_DRAW);
				// 3. then set our vertex attributes pointers
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);

				// ..:: Drawing code (in render loop) :: ..
				// 4. draw the object
				glUseProgram(program);
				glBindVertexArray(VAO);
				//someOpenGLFunctionThatDrawsOurTriangle();
				glUseProgram(program);
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}


			for (auto& s : shaders)
			{
				glDeleteShader(s);
			}
		}
	}
}