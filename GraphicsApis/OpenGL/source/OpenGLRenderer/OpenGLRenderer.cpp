#include <OpenGLRenderer/OpenGLRenderer.h>
#include <iostream>
#include <glm/glm.hpp>
#include <Core/TypeHelpers.h>
#include <algorithm>
#include <OpenGLRenderer/OpenGLMesh.h>
#include <OpenGLRenderer/OpenGLMaterial.h>
#include <ChaiEngine/RenderCommandList.h>

namespace chai::brew
{
	const char* getGLErrorString(GLenum err) {
		switch (err) {
		case GL_NO_ERROR: return "No error";
		case GL_INVALID_ENUM: return "Invalid enum";
		case GL_INVALID_VALUE: return "Invalid value";
		case GL_INVALID_OPERATION: return "Invalid operation";
		case GL_STACK_OVERFLOW: return "Stack overflow";
		case GL_STACK_UNDERFLOW: return "Stack underflow";
		case GL_OUT_OF_MEMORY: return "Out of memory";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid framebuffer operation";
		default: return "Unknown error";
		}
	}

	static void checkGLError(const std::string& context)
	{
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "OpenGL Error (" << context << "): " << getGLErrorString(err) << std::endl;
		}
	}

	OpenGLBackend::OpenGLBackend()
	{
	}

	OpenGLBackend::~OpenGLBackend()
	{
	}

	bool OpenGLBackend::initialize(void* winProcAddress)
	{
		if (!gladLoadGL((GLADloadfunc)winProcAddress))
		{
			std::cerr << "Failed to initialize GLAD" << std::endl;
			checkGLError("setProcAddress");
			return false;
		}

		checkGLError("setProcAddress");

		// Check OpenGL version
		const char* version = (const char*)glGetString(GL_VERSION);
		std::cout << "OpenGL Version: " << version << std::endl;

		// Set default OpenGL state
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		// Enable blending for transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		defaultShaderProgram = createDefaultShaderProgram();
		if (defaultShaderProgram == 0) {
			std::cerr << "Failed to create default shader program" << std::endl;
			return false;
		}

		return true;
	}

	GLuint OpenGLBackend::createDefaultShaderProgram()
	{
		// Simple vertex shader
		const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_transform;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    gl_Position = u_projection * u_view * u_transform * vec4(aPos, 1.0);
}
)";

		const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

		GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
		checkGLError("compile vertex shader");
		GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
		checkGLError("compile frag shader");

		if (vertexShader == 0 || fragmentShader == 0) {
			return 0;
		}

		GLuint program = glCreateProgram();
		checkGLError("create shader program");
		glAttachShader(program, vertexShader);
		checkGLError("attach shader");
		glAttachShader(program, fragmentShader);
		checkGLError("attach shader");
		glLinkProgram(program);
		checkGLError("link shader program");

		// Check linking
		GLint success;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			std::cerr << "Shader program linking failed: " << infoLog << std::endl;
			glDeleteProgram(program);
			return 0;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return program;
	}

	GLuint OpenGLBackend::compileShader(const char* source, GLenum type)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);

		// Check compilation
		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cerr << "Shader compilation failed: " << infoLog << std::endl;
			glDeleteShader(shader);
			return 0;
		}

		return shader;
	}

	void OpenGLBackend::shutdown()
	{
		m_meshCache.clear();
		m_materialCache.clear();

		if (defaultShaderProgram) {
			glDeleteProgram(defaultShaderProgram);
			defaultShaderProgram = 0;
		}
	}

	void OpenGLBackend::executeCommands(const std::vector<RenderCommand>& commands)
	{
		for (const auto& cmd : commands) {
			switch (cmd.type) {
			case RenderCommand::DRAW_MESH:
				drawMesh(cmd);
				break;
			case RenderCommand::SET_VIEWPORT:
				int x, y, width, height;
				cmd.viewport->getViewport(x, y, width, height);
				glViewport(x, y, width, height);
				checkGLError("glViewport");
				break;
			case RenderCommand::CLEAR:
				//this should come from RenderCommand
				clear(0.0f, 0.0f, 0.0f, 1.0f);
				break;
			case RenderCommand::SET_SCISSOR:
				// SetScissor(cmd.scissor.x, cmd.scissor.y, cmd.scissor.width, cmd.scissor.height);
				break;
			}
		}
	}

	void OpenGLBackend::clear(float r, float g, float b, float a) 
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLBackend::drawMesh(const RenderCommand& cmd)
	{
		if (!cmd.mesh) return;

		// Get or create OpenGL mesh data
		OpenGLMeshData* glMeshData = getOrCreateMeshData(cmd.mesh);

		// Upload mesh if not already uploaded
		if (!glMeshData->isUploaded) 
		{
			uploadMeshToGPU(cmd.mesh, glMeshData);
		}

		// Handle material
		GLuint shaderToUse = defaultShaderProgram;
		if (cmd.material) 
		{
			OpenGLMaterialData* glMaterialData = getOrCreateMaterialData(cmd.material.get());
			if (!glMaterialData->isCompiled) {
				compileMaterial(cmd.material.get(), glMaterialData);
			}
			if (glMaterialData->shaderProgram) {
				shaderToUse = glMaterialData->shaderProgram;
			}
		}

		// Bind shader
		bindShaderProgram(shaderToUse);

		// Set transform matrix uniform
		GLint transformLoc = glGetUniformLocation(shaderToUse, "u_transform");
		if (transformLoc != -1) {
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &cmd.transform[0][0]);
		}

		GLint viewLoc = glGetUniformLocation(shaderToUse, "u_view");
		if (viewLoc != -1) {
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &cmd.viewMatrix[0][0]);
		}

		GLint projLoc = glGetUniformLocation(shaderToUse, "u_projection");
		if (projLoc != -1) {
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, &cmd.projectionMatrix[0][0]);
		}

		// Bind and draw mesh
		bindVertexArray(glMeshData->VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(glMeshData->indexCount), GL_UNSIGNED_INT, 0);
		checkGLError("draw mesh");
	}

	OpenGLMeshData* OpenGLBackend::getOrCreateMeshData(IMesh* mesh) 
	{
		auto it = m_meshCache.find(mesh);
		if (it == m_meshCache.end()) {
			auto glMeshData = std::make_unique<OpenGLMeshData>();
			auto* ptr = glMeshData.get();
			m_meshCache[mesh] = std::move(glMeshData);
			return ptr;
		}
		return it->second.get();
	}

	OpenGLMaterialData* OpenGLBackend::getOrCreateMaterialData(IMaterial* material) 
	{
		auto it = m_materialCache.find(material);
		if (it == m_materialCache.end()) {
			auto glMaterialData = std::make_unique<OpenGLMaterialData>();
			auto* ptr = glMaterialData.get();
			m_materialCache[material] = std::move(glMaterialData);
			return ptr;
		}
		return it->second.get();
	}

	void OpenGLBackend::uploadMeshToGPU(IMesh* mesh, OpenGLMeshData* glMeshData) 
	{
		if (glMeshData->isUploaded) return;

		const auto& vertices = mesh->getVertices();
		const auto& indices = mesh->getIndices();

		// Generate buffers
		glGenVertexArrays(1, &glMeshData->VAO);
		glGenBuffers(1, &glMeshData->VBO);
		glGenBuffers(1, &glMeshData->EBO);

		// Bind VAO
		glBindVertexArray(glMeshData->VAO);

		// Upload vertices
		glBindBuffer(GL_ARRAY_BUFFER, glMeshData->VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		// Upload indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glMeshData->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

		// Set up vertex attributes
		setupVertexAttributes();

		glMeshData->indexCount = indices.size();
		glMeshData->isUploaded = true;

		// Unbind
		glBindVertexArray(0);
	}

	void OpenGLBackend::setupVertexAttributes() 
	{
		// Assuming Vertex structure: position(3f), normal(3f), texCoord(2f)
		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);

		// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);

		// Texture coordinate attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
		glEnableVertexAttribArray(2);
	}

	void OpenGLBackend::compileMaterial(IMaterial* material, OpenGLMaterialData* glMaterialData) 
	{
		/*
		std::string vertexSource = material->GetVertexShader();
		std::string fragmentSource = material->GetFragmentShader();

		glMaterialData->shaderProgram = CreateShaderProgram(vertexSource, fragmentSource);
		glMaterialData->isCompiled = true;
		*/

		// Placeholder - use default shader for now
		glMaterialData->shaderProgram = defaultShaderProgram;
		glMaterialData->isCompiled = true;
	}

	void OpenGLBackend::bindShaderProgram(GLuint program) 
	{
		if (currentShaderProgram != program) 
		{
			glUseProgram(program);
			currentShaderProgram = program;
		}
	}

	void OpenGLBackend::bindVertexArray(GLuint vao) 
	{
		if (currentVAO != vao) 
		{
			glBindVertexArray(vao);
			currentVAO = vao;
		}
	}
}