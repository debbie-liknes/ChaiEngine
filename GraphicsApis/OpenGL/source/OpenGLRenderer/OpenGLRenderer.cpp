#include <OpenGLRenderer/OpenGLRenderer.h>
#include <iostream>
#include <Window/Viewport.h>
#include <Window/Window.h>
#include <Renderables/Renderable.h>
#include <OpenGLRenderer/GlPipelineState.h>
#include <glm/glm.hpp>
#include <Core/TypeHelpers.h>
#include <OpenGLRenderer/GLShader.h>
#include <OpenGLRenderer/OpenGLTexture.h>
#include <Resource/ResourceManager.h>

namespace chai::brew
{
	OpenGLBackend::OpenGLBackend()
	{
	}

	OpenGLBackend::~OpenGLBackend()
	{
		for (auto& [ptr, state] : m_renderableStates) {
			if (state.vao) glDeleteVertexArrays(1, &state.vao);
			if (!state.vbos.empty()) glDeleteBuffers(static_cast<GLsizei>(state.vbos.size()), state.vbos.data());
			if (state.ebo) glDeleteBuffers(1, &state.ebo);
		}
		m_renderableStates.clear();
		m_programCache.clear();
		m_ShaderCache.clear();
	}

	int mapTypesToGL(PrimDataType type) {
		switch (type) {
		case PrimDataType::FLOAT: return GL_FLOAT;
		case PrimDataType::INT: return GL_INT;
		case PrimDataType::UNSIGNED_INT: return GL_UNSIGNED_INT;
		default: return GL_FLOAT;
		}
	}

	int toGLPrimitive(PrimitiveMode mode) {
		switch (mode) {
		case PrimitiveMode::POINTS: return GL_POINTS;
		case PrimitiveMode::LINES: return GL_LINES;
		case PrimitiveMode::TRIANGLES: return GL_TRIANGLES;
		default: return GL_TRIANGLES;
		}
	}

	void checkGLError(const std::string& context) {
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "OpenGL error in " << context << ": " << err << std::endl;
		}
	}

	void OpenGLBackend::setProcAddress(void* address)
	{
		//call init on the graphics api instead of doing this here
		if (!gladLoadGLLoader((GLADloadproc)address)) {
			std::cerr << "Failed to initialize GLAD" << std::endl;
		}
		glEnable(GL_CULL_FACE);
		checkGLError("setProcAddress");
	}

	void OpenGLBackend::renderFrame(const RenderFrame& frame)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		static int currentProgram = -1;

		//temporary uniforms
		auto viewUBO = createUniformBuffer<ViewData>(PrimDataType::FLOAT);
		viewUBO->data.projMat = frame.camera.proj;
		viewUBO->data.view = frame.camera.view;
		viewUBO->name = "MatrixData";

		std::vector<std::shared_ptr<UniformBuffer<GPULight>>> lightUBOs;
		for (const auto& light : frame.lights) {
			auto lightUBO = createUniformBuffer<GPULight>(PrimDataType::FLOAT);
			lightUBO->data = light;
			lightUBO->name = "Light";
			lightUBOs.push_back(lightUBO);
		}

		//track what renderables are still used
		//could replace this by tracking adds/removes on the render frame
		std::set<uint64_t> activeRenderableIds;
		for (const auto& ro : frame.renderables) {
			activeRenderableIds.insert(ro->getId());
		}

		//renders
		for (auto& ro : frame.renderables)
		{
			//check the renderable cache
			if(m_renderableStates.find(ro->getId()) == m_renderableStates.end())
			{
				m_renderableStates[ro->getId()] = {};
			}
			GLRenderableState& state = m_renderableStates[ro->getId()];

			if (state.vao == 0 || ro->isDirty()) {
				if (state.vao) glDeleteVertexArrays(1, &state.vao);
				if (!state.vbos.empty()) glDeleteBuffers(static_cast<GLsizei>(state.vbos.size()), state.vbos.data());
				if (state.ebo) glDeleteBuffers(1, &state.ebo);

				glGenVertexArrays(1, &state.vao);
				glBindVertexArray(state.vao);

				state.vbos.resize(ro->m_vertexBuffers.size());
				glGenBuffers(static_cast<GLsizei>(state.vbos.size()), state.vbos.data());

				int i = 0;
				for (const auto& [binding, buffer] : ro->m_vertexBuffers) {
					glBindBuffer(GL_ARRAY_BUFFER, state.vbos[i]);
					glBufferData(GL_ARRAY_BUFFER, buffer->getElementCount() * buffer->getElementSize(), buffer->getRawData(), GL_STATIC_DRAW);
					glVertexAttribPointer(binding, buffer->getNumElementsInType(), mapTypesToGL(buffer->getUnderlyingType()), GL_FALSE, buffer->getElementSize(), nullptr);
					glEnableVertexAttribArray(binding);
					++i;
				}

				if (ro->hasIndexBuffer()) {
					glGenBuffers(1, &state.ebo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state.ebo);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, ro->m_indexBuffer.second->getElementCount() * ro->m_indexBuffer.second->getElementSize(), ro->m_indexBuffer.second->getRawData(), GL_STATIC_DRAW);
				}

				ro->setDirty(false);
				checkGLError("Setup VAO/VBO/EBO");
			}

			glBindVertexArray(state.vao);

			chai::CVector<int> shaderHandles;
			for (const auto& s : ro->m_data) {
				auto shader = LoadOrGetShader(s.shaderSource, s.stage);
				if (!shader) continue;
				shaderHandles.push_back(shader->getHandle());
			}

			auto program = loadOrGetShaderProgram(shaderHandles, ro->m_uniforms);
			if (!program) continue;

			if (program->getProgramHandle() != currentProgram) {
				program->link();
				checkGLError("Program Link");
				program->use();
				checkGLError("Program Use");

				for (const auto& [id, uniform] : ro->m_uniforms) {
					program->addUniform(uniform->name, uniform, id);
				}

				if (ro->m_addViewData) {
					program->addUniform(viewUBO->name, viewUBO, program->getNumUniforms());
				}

				for (auto& ubo : lightUBOs) {
					program->addUniform(ubo->name, ubo, program->getNumUniforms());
				}

				currentProgram = program->getProgramHandle();
			}
			else if (ro->m_addViewData) {
				auto ubo = program->getUniform("MatrixData");
				if (auto view = static_cast<UniformBuffer<ViewData>*>(ubo.get())) {
					view->data.projMat = frame.camera.proj;
					view->data.view = frame.camera.view;
					program->upload("MatrixData");
				}
			}

			if (ro->hasIndexBuffer()) {
				glDrawElements(toGLPrimitive(ro->getPrimitiveType()), ro->m_indexBuffer.second->getElementCount(), mapTypesToGL(ro->m_indexBuffer.second->getUnderlyingType()), nullptr);
			}
			else {
				glDrawArrays(toGLPrimitive(ro->getPrimitiveType()), 0, ro->getVertexCount());
			}
		}

		for (auto it = m_renderableStates.begin(); it != m_renderableStates.end(); ) {
			if (activeRenderableIds.find(it->first) == activeRenderableIds.end()) {
				// Clean up OpenGL resources
				auto& state = it->second;
				if (state.vao) glDeleteVertexArrays(1, &state.vao);
				if (!state.vbos.empty()) glDeleteBuffers(static_cast<GLsizei>(state.vbos.size()), state.vbos.data());
				if (state.ebo) glDeleteBuffers(1, &state.ebo);
				it = m_renderableStates.erase(it);
			}
			else {
				++it;
			}
		}
	}

	GLShader* OpenGLBackend::LoadOrGetShader(const std::string& path, ShaderStage stage)
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
			m_ShaderCache[path] = shader;
			return shader;
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