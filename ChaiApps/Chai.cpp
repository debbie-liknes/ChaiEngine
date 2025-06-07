#include "Chai.h"
#include <Engine/Engine.h>
#include <Window/WindowManager.h>
#include <Window/WindowSystem.h>
#include <Window/Window.h>
#include <Plugin/PluginRegistry.h>

using namespace std;

int main()
{
	//load common plugins
	chai::kettle::PluginRegistry::instance().loadPluginsInDirectory("plugins");
	//chai::kettle::PluginRegistry::instance().get("window", "GLFW");
	//chai::WindowManager windowManager(nullptr);
	//this is the rendering engine
	//chai::brew::Engine engine;
	//engine.init("OpenGLRenderer");

	//chai::WindowDesc desc;
	//desc.title = "Chai Window";
	//desc.width = 1920;
	//desc.height = 1080;
	//desc.fullscreen = false;
	//desc.vsync = true;
	//desc.samples = 4; // 4x MSAA

	//auto mainWindow = windowManager.createWindow(desc);

    //// Optional: Create additional viewports in the same window
    //auto viewport1 = mainWindow->createViewport("MainView", { 0, 0, 1920, 540 });
    //auto viewport2 = mainWindow->createViewport("MiniMap", { 1600, 50, 300, 200 });

    //// Or create separate windows
    //auto debugWindow = engine.createWindow("Debug Tools", 800, 600);
    //auto debugViewport = debugWindow->getDefaultViewport();

	//TODO: engine should only responsible for rendering what we give it, not the main loop
	//engine.run();

	return 0;
}
