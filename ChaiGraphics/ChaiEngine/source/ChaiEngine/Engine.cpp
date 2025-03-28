#include <ChaiEngine/Engine.h>
#include <ChaiEngine/Renderer.h>
#include <Window/Viewport.h>
#include <stdexcept>
#include <glm/glm.hpp>
#include <ChaiEngine/ViewData.h>
#include <Renderables/Cube.h>
#include <Plugin/PluginLoader.h>
#include <Plugin/PluginRegistry.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace chai::brew
{
	using RegisterBackendFn = chai::brew::Renderer* (*)();

	Engine::~Engine()
	{
		//lib cleanup
		shutdown();

		if (nullptr != m_renderer)
		{
			delete m_renderer;
		}
	}

	void Engine::init(std::string backend)
	{
		if (loader.LoadPlugin(backend + ".dll"))
		{
			auto factory = kettle::PluginRegistry::Instance().Get("Renderer", "OpenGL");
			m_renderer = static_cast<Renderer*>(factory());
		}
	}

	void Engine::shutdown() {
		// TODO: clean up objects
	}

	void Engine::createWindow(std::string windowTitle)
	{
		m_windows.push_back(std::make_shared<ChaiWindow>(windowTitle));
		auto& window = m_windows.back();
		chai::SharedViewport testViewport = std::make_shared<chai::Viewport>(0, 0, window->GetWidth(), window->GetHeight());
		window->AddViewport(testViewport);
		m_renderer->setProcAddress(window->getProcAddress());
	}

	void Engine::run()
	{
		chai::brew::ViewData data;
		auto& window = m_windows.back();

		glm::vec3 cameraPos = glm::vec3(0.0f, -5.0f, -5.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

		const float cameraSpeed = 0.01f; // adjust accordingly


		//test ro
		auto cube = std::make_shared<chai::brew::CubeRO>();
		chai::CVector<chai::CSharedPtr<chai::brew::Renderable>> ros = { cube };

		while (window->Show())
		{
			if (window->m_state.keys.find(chai::Key::KEY_W) != window->m_state.keys.end())
				cameraPos += cameraSpeed * cameraFront;
			if (window->m_state.keys.find(chai::Key::KEY_S) != window->m_state.keys.end())
				cameraPos -= cameraSpeed * cameraFront;
			if (window->m_state.keys.find(chai::Key::KEY_A) != window->m_state.keys.end())
				cameraPos -= cameraSpeed * cameraRight;
			if (window->m_state.keys.find(chai::Key::KEY_D) != window->m_state.keys.end())
				cameraPos += cameraSpeed * cameraRight;

			//need a camera class
			glm::mat4 view = glm::lookAt(
				cameraPos,
				glm::vec3(0.0f, 0.0f, 0.0f),  // Target position (center of the scene)
				glm::vec3(0.0f, 1.0f, 0.0f)
			);
			data.view = view;

			float aspectRatio = (float)window->GetWidth() / (float)window->GetHeight();
			glm::mat4 projection = glm::perspective(
				glm::radians(30.0f), // Field of view: 45 degrees
				aspectRatio,         // Aspect ratio: width/height
				0.1f,                // Near clipping plane
				100.0f               // Far clipping plane
			);
			data.projMat = projection;

			window->swapBuffers();
			window->PollEvents();
			m_renderer->renderFrame(window.get(), ros, data);
		}

		window->Close();
	}
}