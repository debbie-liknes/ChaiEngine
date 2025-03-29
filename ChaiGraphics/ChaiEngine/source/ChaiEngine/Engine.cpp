#include <ChaiEngine/Engine.h>
#include <ChaiEngine/Renderer.h>
#include <Window/Viewport.h>
#include <stdexcept>
#include <glm/glm.hpp>
#include <ChaiEngine/ViewData.h>
#include <Renderables/Cube.h>
#include <Plugin/PluginLoader.h>
#include <Plugin/PluginRegistry.h>
#include <Resource/ResourceManager.h>
#include <Plugin/ServiceLocator.h>
#include <Scene/Camera.h>

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
		ServiceLocator::getInstance().Shutdown();
		//lib cleanup
		shutdown();
	}

	Renderer& Engine::renderer()
	{
		return *m_renderer;
	}

	void Engine::init(std::string backend)
	{
		//possible TODO: auto load plugins?
		//these should at minimum be loaded somewhere else
		//and these plugins should be more swappable
		if (loader.LoadPlugin(backend + ".dll"))
		{
			auto factory = kettle::PluginRegistry::Instance().Get("Renderer", "OpenGL");
			m_renderer = std::shared_ptr<Renderer>(static_cast<Renderer*>(factory()));
			ServiceLocator::getInstance().Register<Renderer>(std::shared_ptr<Renderer>(m_renderer));
		}

		if (loader.LoadPlugin("TextureLoader.dll"))
		{
			auto factory = kettle::PluginRegistry::Instance().Get("Loader", "TextureLoader");
			auto loader = static_cast<IResourceLoader*>(factory());
			if (loader)
			{
				chai::ResourceManager::Instance().RegisterLoader(std::shared_ptr<IResourceLoader>(loader));
			}
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

		const float cameraSpeed = 0.01f; // adjust accordingly

		float aspectRatio = (float)window->GetWidth() / (float)window->GetHeight();
		cup::Camera cam;
		cam.SetPerspective(30.f, aspectRatio, 0.1f, 100.f);
		cam.SetPosition(glm::vec3(0.0f, 0.0f, -5.0f));
		cam.lookAt(glm::vec3(0.f, 0.f, 0.f));

		//test ro
		auto cube = std::make_shared<chai::brew::CubeRO>();
		chai::CVector<chai::CSharedPtr<chai::brew::Renderable>> ros = { cube };

		while (window->Show())
		{
			if (window->m_state.keys.find(chai::Key::KEY_W) != window->m_state.keys.end())
				cam.SetPosition(cam.getPosition() + (cameraSpeed * cam.getDirection()));
			if (window->m_state.keys.find(chai::Key::KEY_S) != window->m_state.keys.end())
				cam.SetPosition(cam.getPosition() - (cameraSpeed * cam.getDirection()));
			if (window->m_state.keys.find(chai::Key::KEY_A) != window->m_state.keys.end())
				cam.SetPosition(cam.getPosition() + (cameraSpeed * cam.getRight()));
			if (window->m_state.keys.find(chai::Key::KEY_D) != window->m_state.keys.end())
				cam.SetPosition(cam.getPosition() - (cameraSpeed * cam.getRight()));

			data.view = cam.GetViewMatrix();

			data.projMat = cam.GetProjectionMatrix();

			window->swapBuffers();
			window->PollEvents();
			m_renderer->renderFrame(window.get(), ros, data);
		}

		window->Close();
	}
}