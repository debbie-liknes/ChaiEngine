#include <Engine/Engine.h>
#include <ChaiEngine/Renderer.h>
#include <Window/Viewport.h>
#include <stdexcept>
#include <glm/glm.hpp>
#include <ChaiEngine/ViewData.h>
#include <Plugin/PluginLoader.h>
#include <Plugin/PluginRegistry.h>
#include <Resource/ResourceManager.h>
#include <Plugin/ServiceLocator.h>
#include <Meta/TypeRegistry.h>
#include <Scene/RenderFrameBuilder.h>

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
			auto shaderLoader = kettle::PluginRegistry::Instance().Get("ShaderResourceLoader", "OpenGL");
			auto loader = static_cast<IResourceLoader*>(shaderLoader());

			ServiceLocator::getInstance().Register<Renderer>(std::shared_ptr<Renderer>(m_renderer));
			chai::ResourceManager::Instance().RegisterLoader(std::shared_ptr<IResourceLoader>(loader));
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

		m_audioEngine->Init();
	}

	void Engine::shutdown() {
		// TODO: clean up objects
		m_audioEngine->Shutdown();
	}

	void Engine::createWindow(std::string windowTitle)
	{
		m_windows.push_back(std::make_shared<ChaiWindow>(windowTitle));
		auto& window = m_windows.back();

		//TODO: get this string from elsewhere
		auto info = chai::kettle::TypeRegistry::Instance().Get("chai::brew::GLViewport");
		void* raw = info->factory();
		auto* viewport = static_cast<Viewport*>(raw);

		viewport->setDimensions(0, 0, window->GetWidth(), window->GetHeight());
		window->AddViewport(std::shared_ptr<Viewport>(viewport));
		m_renderer->setProcAddress(window->getProcAddress());
	}

	void Engine::run()
	{
		auto& window = m_windows.back();

		const float cameraSpeed = 0.003f; // adjust accordingly

		m_scene = std::make_shared<cup::Scene>();

		m_audioEngine->LoadSound("C:/Users/Lucius/Music/campfirefireplace-crackling-268525.mp3",
			true, true, true);
		m_audioEngine->PlaySound("C:/Users/Lucius/Music/campfirefireplace-crackling-268525.mp3",
			glm::vec3{ 0, 0, 0 }, 3.0F);

		while (window->Show())
		{
			//TODO: input system
			if (window->m_state.keys.find(chai::Key::KEY_W) != window->m_state.keys.end())
				m_scene->m_cam.SetPosition(m_scene->m_cam.getPosition() + (cameraSpeed * m_scene->m_cam.getDirection()));
			if (window->m_state.keys.find(chai::Key::KEY_S) != window->m_state.keys.end())
				m_scene->m_cam.SetPosition(m_scene->m_cam.getPosition() - (cameraSpeed * m_scene->m_cam.getDirection()));
			if (window->m_state.keys.find(chai::Key::KEY_A) != window->m_state.keys.end())
				m_scene->m_cam.SetPosition(m_scene->m_cam.getPosition() - (cameraSpeed * m_scene->m_cam.getRight()));
			if (window->m_state.keys.find(chai::Key::KEY_D) != window->m_state.keys.end())
				m_scene->m_cam.SetPosition(m_scene->m_cam.getPosition() + (cameraSpeed * m_scene->m_cam.getRight()));
			if (window->m_state.keys.find(chai::Key::KEY_Q) != window->m_state.keys.end())
				m_scene->m_cam.SetPosition(m_scene->m_cam.getPosition() - (cameraSpeed * m_scene->m_cam.getUp()));
			if (window->m_state.keys.find(chai::Key::KEY_E) != window->m_state.keys.end())
				m_scene->m_cam.SetPosition(m_scene->m_cam.getPosition() + (cameraSpeed * m_scene->m_cam.getUp()));

			if (window->m_state.mouseButtons.find(MouseButton::LEFT) != window->m_state.mouseButtons.end())
			{
				float sensitivity = 0.1f;
				float y, p, r;
				m_scene->m_cam.getYawPitchRoll(y, p, r);
				float yaw = y + (window->m_state.mouseDiff.x * sensitivity);
				float pitch = p - (window->m_state.mouseDiff.y * sensitivity);
				m_scene->m_cam.setYawPitch(yaw, pitch);
			}

			//clear state
			window->m_state.mouseDiff = glm::vec2{ 0.f, 0.f };

			window->swapBuffers();
			window->PollEvents();

			//this is a bit muddy, atm
			//who owns the scene? independent of viewports?
			//probably need a viewport manager
			for (auto vp : window->GetViewports())
			{
				auto renderFrame = cup::RenderFrameBuilder::build(*m_scene, vp.get());
				vp->bind();
				m_renderer->renderFrame(renderFrame);
			}

			// Update listener position to camera
			m_audioEngine->Set3dListenerAndOrientation(
				m_scene->m_cam.getPosition(),
				m_scene->m_cam.getDirection(),
				m_scene->m_cam.getUp());
			m_audioEngine->Update();
		}

		window->Close();
	}
}