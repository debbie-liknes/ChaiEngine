#include <OpenGLRenderer/OpenGLRenderer.h>
#include <glad/glad.h>
#include <iostream>
#include <Engine/Viewport.h>
#include <Engine/Window.h>
#include <RenderObjects/RenderObject.h>
#include <OpenGLRenderer/GlPipelineState.h>
#include <glm/glm.hpp>
#include <algorithm>

namespace CGraphics
{
	OpenGLBackend::OpenGLBackend()
	{
		m_pipelineState = std::make_shared<GlPipelineState>();
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

	void mapTypesToGL()
	{
		GL_FLOAT;
		GL_INT;
		GL_UNSIGNED_INT;
	}

	void setUpVBOs(Core::CVector<unsigned int>& vbos, Core::CVector<Core::CSharedPtr<VertexBufferBase>> vbs)
	{
		//gen buffers is expensive, only do when necessary
		glGenBuffers(vbos.size(), vbos.data());
		for (int i = 0; i < vbos.size(); i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
			glBufferData(GL_ARRAY_BUFFER, vbs[i]->getElementCount() * vbs[i]->getElementSize(), vbs[i]->getRawData(), GL_STATIC_DRAW);
		}
	}

	void setUpVAOs(std::map<uint16_t, Core::CSharedPtr<CGraphics::VertexBufferBase>>& vbos)
	{
		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//rn i have different vbos for different attributes
		//should support interleaved data with strides
		for (auto& vbInfo : vbos)
		{
			auto binding = vbInfo.first;
			auto& vb = vbInfo.second;
			glVertexAttribPointer(binding, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
		}

		//glBindVertexArray(0); // Unbind VAO
	}

	void setUpShaders(Core::CVector<unsigned int>& vbos, Core::CSharedPtr<VertexBufferBase> vb)
	{
		//only needs to be done
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
			Core::CVector<unsigned int> numBuffs;
			numBuffs.resize(ro->m_vertexBuffers.size());

			if (ro->isDirty())
			{
				std::vector<Core::CSharedPtr<VertexBufferBase>> values;
				std::transform(ro->m_vertexBuffers.begin(), ro->m_vertexBuffers.end(), std::back_inserter(values),
					[](std::pair<uint16_t, Core::CSharedPtr<VertexBufferBase>> kv) { return kv.second; });

				setUpVBOs(numBuffs, values);
				setUpVAOs(ro->m_vertexBuffers);
				ro->setDirty(false);
			}

			Core::CVector<int> shaders;
			for (auto& s : ro->m_data)
			{
				shaders.push_back(createShader(s.shaderSource.data(), s.stage));
			}

			int program = createShaderProgram(shaders);

			glUseProgram(program);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			for (auto& s : shaders)
			{
				glDeleteShader(s);
			}
		}
	}
}