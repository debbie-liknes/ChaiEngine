#include <OpenGLRenderer/OpenGLRenderer.h>
#include <iostream>
//#include <ChaiEngine/Viewport.h>
#include <ChaiEngine/Window.h>
#include <Renderables/Renderable.h>
#include <glm/glm.hpp>

#include <Core/TypeHelpers.h>
#include <OpenGLRenderer/GLShader.h>
#include <OpenGLRenderer/OpenGLTexture.h>
#include <Resource/ResourceManager.h>
#include <algorithm>
#include <OpenGLRenderer/OpenGLMesh.h>
#include <OpenGLRenderer/OpenGLMaterial.h>

namespace chai::brew
{
	static void checkGLError(const std::string& context)
	{
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "OpenGL error in " << context << ": " << err << std::endl;
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

		return true;
	}

	void OpenGLBackend::shutdown()
	{
		// Cleanup OpenGL resources
		currentMaterial = nullptr;
		currentVAO = 0;
	}

	void OpenGLBackend::executeCommands(const std::vector<RenderCommand>& commands)
	{
		// Reset state tracking
		currentMaterial = nullptr;
		currentVAO = 0;

		// Sort commands by type and material for batching
		std::vector<const RenderCommand*> sortedCommands;
		sortedCommands.reserve(commands.size());

		for (const auto& cmd : commands) {
			sortedCommands.push_back(&cmd);
		}

		// Sort: Clear/Setup commands first, then by material for batching
		std::sort(sortedCommands.begin(), sortedCommands.end(),
			[](const RenderCommand* a, const RenderCommand* b) {
				// Non-draw commands first
				if (a->type != RenderCommand::DRAW_MESH && b->type == RenderCommand::DRAW_MESH) {
					return true;
				}
				if (a->type == RenderCommand::DRAW_MESH && b->type != RenderCommand::DRAW_MESH) {
					return false;
				}

				// Both are draw commands - sort by material, then by render queue
				if (a->type == RenderCommand::DRAW_MESH && b->type == RenderCommand::DRAW_MESH) {
					if (a->material.get() != b->material.get()) {
						return a->material.get() < b->material.get();
					}
					return a->material->getRenderQueue() < b->material->getRenderQueue();
				}

				return false;
			});

		// Execute sorted commands
		//for (const RenderCommand* cmd : sortedCommands) {
		//	switch (cmd->type) {
		//	case RenderCommand::DRAW_MESH:
		//		executeDrawMesh(*cmd);
		//		break;
		//	case RenderCommand::SET_VIEWPORT:
		//		executeSetViewport(*cmd);
		//		break;
		//	case RenderCommand::CLEAR:
		//		executeClear(*cmd);
		//		break;
		//	default:
		//		std::cerr << "Unknown render command type\n";
		//		break;
		//	}
		//}

		// Check for OpenGL errors
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL error after command execution: " << error << std::endl;
		}
	}

	// Resource factories
	std::shared_ptr<IMesh> OpenGLBackend::createMesh(const std::vector<Vertex>& vertices,const std::vector<uint32_t>& indices)
	{
		return std::make_shared<OpenGLMesh>(vertices, indices);
	}

	std::shared_ptr<IMaterial> OpenGLBackend::createMaterial(const std::string& vertexShader, const std::string& fragmentShader)
	{
		return std::make_shared<OpenGLMaterial>(vertexShader, fragmentShader);
	}

	// Texture management
	uint32_t OpenGLBackend::createTexture(const void* data, int width, int height, int channels)
	{
		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// Determine format
		GLenum format, internalFormat;
		switch (channels) {
		case 1:
			format = internalFormat = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			internalFormat = GL_RGB8;
			break;
		case 4:
			format = GL_RGBA;
			internalFormat = GL_RGBA8;
			break;
		default:
			std::cerr << "Unsupported texture format with " << channels << " channels\n";
			glDeleteTextures(1, &textureId);
			return 0;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		return textureId;
	}
}