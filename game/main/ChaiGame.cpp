#include <Runtime/Engine.h>
#include <OS/SystemPaths.h>
#include <Log.h>
#include <Plugin/PluginLoader.h>
#include <tracy/Tracy.hpp>

int main()
{
    // Required to initialize tracy
    TracyNoop;

    using namespace chai;

    Engine engine;

	auto exeDir = executableDir();
	if (exeDir.empty())
		exeDir = std::filesystem::current_path();
    PluginLoader loader;
	if (!loader.loadDirectory(exeDir / "plugins")) {
		CHAI_LOG_CRITICAL("Editor: Failed to load plugins. Exiting prematurely.");
		return 1;
	}

	engine.setPlugins(loader.plugins());
	if (!engine.startup())
	{
		CHAI_LOG_CRITICAL("Engine failed to start. Exiting prematurely.");
		return 1;
	}

    engine.run([](const UpdateContext&){});

    engine.shutdown();

    return 0;
}
