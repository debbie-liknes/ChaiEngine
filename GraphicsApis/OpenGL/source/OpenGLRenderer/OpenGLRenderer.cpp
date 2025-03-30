#include <OpenGLRenderer/OpenGLRenderer.h>
#include <glad/glad.h>
#include <iostream>
#include <Window/Viewport.h>
#include <Window/Window.h>
#include <Renderables/Renderable.h>
#include <OpenGLRenderer/GlPipelineState.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <Core/TypeHelpers.h>
#include <fstream>
#include <sstream>
#include <OpenGLRenderer/GLShader.h>
#include <OpenGLRenderer/OpenGLTexture.h>

namespace chai::brew
{
	OpenGLBackend::OpenGLBackend()
	{
		//m_pipelineState = std::make_shared<GlPipelineState>();
	}

	void OpenGLBackend::setProcAddress(void* address)
	{
		//call init on the graphics api instead of doing this here
		if (!gladLoadGLLoader((GLADloadproc)address))
		{
			std::cerr << "Failed to initialize GLAD" << std::endl;
		}
		glEnable(GL_CULL_FACE);
	}

	int OpenGLBackend::createShaderProgram(chai::CVector<int> shaders)
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

	void setUpVBOs(chai::CVector<unsigned int>& glVbs, std::map<uint16_t, chai::CSharedPtr<VertexBufferBase>> vbs)
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

	void setUpEBOs(chai::CSharedPtr<VertexBufferBase> ib)
	{
		unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib->getElementCount() * ib->getElementSize(), ib->getRawData(), GL_STATIC_DRAW);
	}

	unsigned int setUpVAOs(std::map<uint16_t, chai::CSharedPtr<VertexBufferBase>>& vbos)
	{
		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		return vao;
	}

	void setUpUniforms(int shaderProgram, std::map<uint16_t, chai::CSharedPtr<UniformBufferBase>>& ubos)
	{
		chai::CVector<unsigned int> uboMatrices;
		uboMatrices.resize(ubos.size());
		glGenBuffers(ubos.size(), uboMatrices.data());
		int i = 0;
		for (auto& ub : ubos)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices[i]);
			// Allocate memory for the UBO (size of 3 matrices)
			glBufferData(GL_UNIFORM_BUFFER, ub.second->getElementSize(), ub.second->getRawData(), GL_STATIC_DRAW);

			GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, ub.second->name.c_str());
			glUniformBlockBinding(shaderProgram, blockIndex, ub.first);
			glBindBufferBase(GL_UNIFORM_BUFFER, ub.first, uboMatrices[i]);

			i++;
		}
	}

	void setUpShaders(chai::CVector<unsigned int>& vbos, chai::CSharedPtr<VertexBufferBase> vb)
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

	void OpenGLBackend::renderFrame(const RenderFrame& frame)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto viewUBO = createUniformBuffer<ViewData>(PrimDataType::FLOAT);
		viewUBO->data.projMat = frame.camera.proj;
		viewUBO->data.view = frame.camera.view;
		viewUBO->name = "MatrixData";

		//renders
		//this is not efficient, atm
		for (auto& ro : frame.renderables)
		{
			chai::CVector<unsigned int> numBuffs;
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

			chai::CVector<int> shaders;
			chai::CVector<GLShader*> shadersObj;
			int shaderProgram;
			GLShader* glShader;
			for (auto& s : ro->m_data)
			{
				//this is the individual shader
				auto shader = LoadOrGetShader(s.shaderSource, s.stage);
				glShader = static_cast<GLShader*>(shader.get());
				shaders.push_back(glShader->getHandle());
				shadersObj.push_back(glShader);
			}

			auto program = loadOrGetShaderProgram(shaders);

			auto unis = ro->m_uniforms;
			if (ro->m_addViewData)
			{
				unis.insert({ unis.size(), viewUBO});
			}


			glUseProgram(program->getProgramHandle());
			setUpUniforms(program->getProgramHandle(), unis);

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

	std::shared_ptr<Shader> OpenGLBackend::LoadOrGetShader(const std::string& path, ShaderStage stage)
	{
		//this is terrible, i need a resource manager
		auto it = m_ShaderCache.find(path);
		if (it != m_ShaderCache.end())
		{
			return m_ShaderCache[path];
		}

		//we dont have it yet
		auto shader = std::make_shared<GLShader>();
		std::string fullpath = std::string(CMAKE_SOURCE_DIR) + "/" + path;
		std::ifstream shaderFile(fullpath);
		std::stringstream buffer;
		buffer << shaderFile.rdbuf();
		shader->shaderSource = buffer.str();
		m_ShaderCache[path] = shader;
		shader->createShader(shader->shaderSource.data(), stage);

		return shader;
	}

	std::shared_ptr<GLShaderProgram> OpenGLBackend::loadOrGetShaderProgram(std::vector<int> shaders)
	{
		for (auto& program : m_programCache)
		{
			auto progShaders = program->getShaders();
			if (shaders.size() != progShaders.size())
				continue;

			bool found = true;
			for (auto& s : shaders)
			{
				if (progShaders.find(s) == progShaders.end())
				{
					found = false;
					break;
				}
			}
			if (found)
			{
				return program;
			}
		}

		int program = createShaderProgram(shaders);
		auto glProg = std::make_shared<GLShaderProgram>(program);
		for (auto& s : shaders)
		{
			glProg->AddShader(s);
		}
		m_programCache.push_back(std::make_shared<GLShaderProgram>(program));
		return glProg;
	}

	std::shared_ptr<GLShader> OpenGLBackend::getShaderByHandle(int shader)
	{
		for (auto& s : m_ShaderCache)
		{
			if (s.second->getHandle() == shader)
				return s.second;
		}
		return nullptr;
	}

	std::shared_ptr<ITextureBackend> OpenGLBackend::createTexture2D(const uint8_t* data, uint32_t width, uint32_t height)
	{
		return std::make_shared<OpenGLTextureBackend>();
	}
}