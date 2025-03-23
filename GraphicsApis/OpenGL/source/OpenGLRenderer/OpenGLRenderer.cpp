#include <OpenGLRenderer/OpenGLRenderer.h>
#include <glad/glad.h>
#include <iostream>
#include <Window/Viewport.h>
#include <Window/Window.h>
#include <RenderObjects/RenderObject.h>
#include <OpenGLRenderer/GlPipelineState.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <Core/TypeHelpers.h>

using namespace chai;

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

	int mapTypesToGL(PrimDataType type)
	{
		switch (type)
		{
		case PrimDataType::FLOAT:
			return GL_FLOAT;
		case PrimDataType::INT:
			return GL_INT;
		case PrimDataType::UNSIGNED_INT:
			return GL_UNSIGNED_INT;
		default:
			break;
		}
		return GL_FLOAT;
	}

	void setUpVBOs(Core::CVector<unsigned int>& glVbs, std::map<uint16_t, Core::CSharedPtr<VertexBufferBase>> vbs)
	{
		//an assert here?
		if (glVbs.size() != vbs.size()) return;
		//gen buffers is expensive, only do when necessary
		//should really only do this on init
		glGenBuffers(glVbs.size(), glVbs.data());
		int i = 0;
		for (auto vb : vbs)
		{
			uint32_t binding = vb.first;
			auto buffer = vb.second;
			glBindBuffer(GL_ARRAY_BUFFER, glVbs[i]);
			glBufferData(GL_ARRAY_BUFFER, vbs[i]->getElementCount() * vbs[i]->getElementSize(), vbs[i]->getRawData(), GL_STATIC_DRAW);
			
			glVertexAttribPointer(binding, buffer->getNumElementsInType(), mapTypesToGL(buffer->getUnderlyingType()), GL_FALSE, buffer->getElementSize(), (void*)0);
			glEnableVertexAttribArray(binding);

			i++;
		}
	}

	void setUpEBOs(Core::CSharedPtr<VertexBufferBase> ib)
	{
		unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib->getElementCount() * ib->getElementSize(), ib->getRawData(), GL_STATIC_DRAW);
	}

	unsigned int setUpVAOs(std::map<uint16_t, Core::CSharedPtr<CGraphics::VertexBufferBase>>& vbos)
	{
		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		return vao;
	}

	void setUpUniforms(int shaderProgram, std::map<uint16_t, Core::CSharedPtr<CGraphics::UniformBufferBase>>& ubos)
	{
		Core::CVector<unsigned int> uboMatrices;
		uboMatrices.resize(ubos.size());
		glGenBuffers(ubos.size(), uboMatrices.data());
		int i = 0;
		for (auto& ub : ubos)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices[i]);
			// Allocate memory for the UBO (size of 3 matrices)
			glBufferData(GL_UNIFORM_BUFFER, ub.second->getElementSize(), ub.second->getRawData(), GL_STATIC_DRAW);

			// Bind the UBO to a binding point (e.g., binding point 0)
			glBindBufferBase(GL_UNIFORM_BUFFER, i, uboMatrices[i]);

			int ubLocation = glGetUniformLocation(shaderProgram, ub.second->name.c_str());
			glUniformBlockBinding(shaderProgram, ubLocation, i);
			i++;
		}
	}

	void setUpShaders(Core::CVector<unsigned int>& vbos, Core::CSharedPtr<VertexBufferBase> vb)
	{
		//only needs to be done
	}

	int toGLPrimitive(PrimitiveMode mode)
	{
		switch (mode)
		{
		case PrimitiveMode::POINTS:
			return GL_POINTS;
		case PrimitiveMode::LINES:
			return GL_LINES;
		case PrimitiveMode::TRIANGLES:
			return GL_TRIANGLES;
		default:
			break;
		}
		return GL_TRIANGLES;
	}

	void OpenGLBackend::renderFrame(Window* window, Core::CVector<Core::CSharedPtr<RenderObject>> ros, chai_graphics::ViewData data)
	{
		for (auto& v : window->GetViewports())
		{
			int x, y, width, height;
			v->GetDimensions(x, y, width, height);
			glViewport(x, y, width, height);
		}

		auto viewUBO = createUniformBuffer<chai_graphics::ViewData>(PrimDataType::FLOAT);
		viewUBO->data = data;
		viewUBO->name = "MatrixData";

		//renders
		//this is not efficient, atm
		for (auto& ro : ros)
		{
			Core::CVector<unsigned int> numBuffs;
			numBuffs.resize(ro->m_vertexBuffers.size());

			if (ro->isDirty())
			{
				setUpVAOs(ro->m_vertexBuffers);
				setUpVBOs(numBuffs, ro->m_vertexBuffers);
				if (ro->hasIndexBuffer())
				{
					setUpEBOs(ro->m_indexBuffer.second);
				}
				ro->setDirty(false);
			}

			Core::CVector<int> shaders;
			for (auto& s : ro->m_data)
			{
				shaders.push_back(createShader(s.shaderSource.data(), s.stage));
			}

			int program = createShaderProgram(shaders);

			if (ro->m_addViewData)
			{
				ro->m_uniforms.insert({ ro->m_uniforms.size(), viewUBO });
				ro->m_addViewData = false;
			}

			setUpUniforms(program, ro->m_uniforms);

			glUseProgram(program);

			if (ro->hasIndexBuffer())
			{
				glDrawElements(toGLPrimitive(ro->getPrimitiveType()), ro->m_indexBuffer.second->getElementCount(), mapTypesToGL(ro->m_indexBuffer.second->getUnderlyingType()), (void*)0);
			}
			else
			{
				glDrawArrays(toGLPrimitive(ro->getPrimitiveType()), 0, 3);
			}

			for (auto& s : shaders)
			{
				glDeleteShader(s);
			}
		}
	}
}