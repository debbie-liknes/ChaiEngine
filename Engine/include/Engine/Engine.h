#pragma once
#include <string>
#include <memory>
#include <Resource/ResourceLoader.h>
#include <Scene/Scene.h>
#include <AudioEngine.h>

namespace chai::brew
{
	class Renderer;
	class Engine
	{
	public:
		Engine() = default;
		virtual ~Engine();

		void init(std::string backend);
		void run();

		//TODO: this should be in a window manager
		//std::shared_ptr<Window> createWindow(std::string windowTitle);

		Renderer& renderer();

	private:

		//windows and viewport belong in some kind of window manager
		//std::vector<std::shared_ptr<ChaiWindow>> m_windows;

		std::shared_ptr<Renderer> m_renderer;
		//test scene
		std::shared_ptr<cup::Scene> m_scene;

		std::shared_ptr<AudioEngine> m_audioEngine;

		void shutdown();
	};
}