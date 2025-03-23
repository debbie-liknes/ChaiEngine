#pragma once
#include <ChaiGraphicsExport.h>
#include <string>
#include <memory>
#include <Window/GLFWImpl.h>

namespace chai::brew
{
	class Renderer;
	class CHAIGRAPHICS_EXPORT Engine
	{
	public:
		Engine() = default;
		virtual ~Engine();

		void init(std::string backend);
		void run();

		void createWindow(std::string windowTitle);

	private:
		void* m_backendLibHandle = nullptr;

		//windows and viewport belong in some kind of window manager
		std::vector<std::shared_ptr<ChaiWindow>> m_windows;

		Renderer* m_renderer;

		void shutdown();
	};
}