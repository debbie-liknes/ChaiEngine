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
#include <Resource/ResourceManager.h>

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
		for (auto& vb : vbs)
		{
			uint32_t binding = vb.first;
			auto& buffer = vb.second;
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

		static int currentProgram = -1;
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
			int shaderProgram;
			GLShader* glShader;
			for (auto& s : ro->m_data)
			{
				//this is the individual shader
				auto shader = LoadOrGetShader(s.shaderSource, s.stage);
				glShader = static_cast<GLShader*>(shader.get());
				shaders.push_back(glShader->getHandle());
			}

			auto program = loadOrGetShaderProgram(shaders, ro->m_uniforms);

			//must call use before setting up uniforms
			if (program->getProgramHandle() != currentProgram)
			{
				program->link();
				program->use();

				//add the renderable uniforms to the program
				for (auto& uni : ro->m_uniforms)
				{
					program->addUniform(uni.second->name, uni.second, uni.first);
				}

				if (ro->m_addViewData)
				{
					auto viewUBO = createUniformBuffer<ViewData>(PrimDataType::FLOAT);
					viewUBO->data.projMat = frame.camera.proj;
					viewUBO->data.view = frame.camera.view;
					viewUBO->name = "MatrixData";
					program->addUniform(viewUBO->name, viewUBO, program->getNumUniforms());
				}

				//there should only be one, right now
				//TODO: make a lightweight lighting system
				for (auto& light : frame.lights)
				{
					auto lightUBO = createUniformBuffer<GPULight>(PrimDataType::FLOAT);
					lightUBO->data.color = light.color;
					lightUBO->data.position = light.position;
					lightUBO->data.intensity = light.intensity;
					lightUBO->name = "Light";
					program->addUniform(lightUBO->name, lightUBO, program->getNumUniforms());
				}

				currentProgram = program->getProgramHandle();
			}
			//check if we should add view data
			else if (ro->m_addViewData)
			{
				auto viewUBO = program->getUniform("MatrixData");
				auto view = static_cast<UniformBuffer<ViewData>*>(viewUBO.get());
				if (view)
				{
					view->data.projMat = frame.camera.proj;
					view->data.view = frame.camera.view;
				}
				program->upload("MatrixData");
			}

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
		auto it = m_ShaderCache.find(path);
		if (it != m_ShaderCache.end())
		{
			return m_ShaderCache[path];
		}

		//we dont have it yet
		std::shared_ptr<IResource> resource = chai::ResourceManager::Instance().Load(path);
		auto shaderResource = static_cast<ShaderResource*>(resource.get());
		if (shaderResource && shaderResource->shader)
		{
			auto shader = static_cast<GLShader*>(shaderResource->shader.get());
			if (shader)
			{
				shader->createShader(shader->shaderSource.data(), stage);
			}
			m_ShaderCache[path] = std::shared_ptr<GLShader>(shader);
			return shaderResource->shader;
		}
		return nullptr;
	}

	std::shared_ptr<GLShaderProgram> OpenGLBackend::loadOrGetShaderProgram(std::vector<int> shaders, std::map<uint16_t, chai::CSharedPtr<UniformBufferBase>> ubos)
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

		auto glProg = std::make_shared<GLShaderProgram>();

		//add shaders to the program
		for (auto& s : shaders)
		{
			glProg->AddShader(s);
		}

		m_programCache.push_back(glProg);
		return glProg;
	}

	std::shared_ptr<ITextureBackend> OpenGLBackend::createTexture2D(const uint8_t* data, uint32_t width, uint32_t height)
	{
		return std::make_shared<OpenGLTextureBackend>();
	}
}