#include <Engine.h>
#include <SpdLogSink.h>
#include <Log.h>
#include <Window/Window.h>
#include <Plugin/PluginLoader.h>
#include <SystemPaths.h>
#include <Renderer.h>

int main()
{
    using namespace chai;

    //setup logging
    SpdlogSink logSink; 
    setLogSink(&logSink);
    setLogLevel(chai::LogLevel::Info);

    chai::Engine engine;

    //Load plugins
    chai::PluginLoader loader;
    auto exeDir = chai::executableDir();
    if (exeDir.empty())
        exeDir = std::filesystem::current_path();
    loader.loadDirectory(exeDir / "plugins");
    engine.setPlugins(loader.plugins());

    engine.startup();

    //Try to make a window
    auto win = engine.services().tryResolve<IWindow>();
    if (!win)
    {
        CHAI_LOG_CRITICAL("Could not locate Window Service.");
        return 1;
    }

    auto renderer = engine.services().tryResolve<gfx::IRenderer>();
    if (!renderer)
    {
        CHAI_LOG_CRITICAL("Could not locate Renderer.");
        return 1;
    }

    //main loop
    while (!win->shouldClose()) {
        auto events = win->pollEvents();
        renderer->renderFrame();
        engine.tick();
    }

    win.reset();

    engine.shutdown();
}