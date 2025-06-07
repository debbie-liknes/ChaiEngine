#include <Window/WindowManager.h>
#include <Window/WindowSystem.h>

namespace chai
{
	WindowManager::WindowManager(std::unique_ptr<WindowSystem> system) : m_windowSystem(std::move(system))
	{
		//Other init
	}
	WindowManager::~WindowManager()
	{
		// Cleanup window system and resources
	}
	//bool WindowManager::initialize()
	//{
	//    // Initialize the window system
	//}
	//void WindowManager::shutdown()
	//{
	//    // Shutdown the window system and clean up resources
	//}
	//void WindowManager::update()
	//{
	//    // Update all windows and handle events
	//}
}