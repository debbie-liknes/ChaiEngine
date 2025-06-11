#include "Chai.h"
#include <Engine/Engine.h>
#include <Window/WindowManager.h>
#include <Window/WindowSystem.h>
#include <Window/Window.h>
#include <Plugin/PluginRegistry.h>
#include <Plugin/ServiceLocator.h>
#include <ChaiEngine/Renderer.h>
#include <Window/ViewportManager.h>
#include <Scene/GameObject.h>
#include <Scene/MeshComponent.h>

using namespace std;

int main()
{
	//load common plugins
	chai::kettle::PluginRegistry::instance().loadPluginsInDirectory("plugins");

	//create window system and manager
	auto windowSystem = chai::ServiceLocator::instance().get<chai::WindowSystem>();
	auto windowManager = std::make_unique<chai::WindowManager>(windowSystem);
	chai::ViewportManager viewportManager;

	chai::WindowDesc desc;
	desc.title = "Chai Window";
	desc.width = 1920;
	desc.height = 1080;
	desc.fullscreen = false;
	desc.vsync = true;
	desc.samples = 4; // 4x MSAA

	//create the top level OS container
	auto mainWindow = windowManager->createWindow(desc);

	//this is the renderer
	auto renderer = chai::ServiceLocator::instance().get<chai::brew::Renderer>();
	renderer->initialize(windowSystem->getProcAddress());

	//create viewports to go inside the window
	viewportManager.createViewport(mainWindow, { "MainView", 0, 0, 1920, 1080 });

	//make a scene
	auto gameObject = std::make_unique<chai::cup::GameObject>();
	//gameObject->addComponent<chai::cup::Cube>();

	while (!windowManager->isDone())
	{
		windowManager->update();
	}

	renderer->shutdown();

	return 0;
}
