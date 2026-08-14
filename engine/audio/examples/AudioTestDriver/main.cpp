#include <Runtime/Engine.h>
#include <Plugin/PluginLoader.h>
#include <OS/SystemPaths.h>
#include <Audio/IAudioEngine.h>

std::filesystem::path assetDir()
{
    return CHAI_ASSET_DIR;
}

int main()
{
    using namespace chai;

    Engine engine;
    PluginLoader loader;
    auto exeDir = executableDir();
    if (exeDir.empty())
        exeDir = std::filesystem::current_path();
    loader.loadDirectory(exeDir / "plugins");
    engine.setPlugins(loader.plugins());
    engine.startup();

    auto audio = engine.services().tryResolve<audio::IAudioEngine>();
    audio->playSound((assetDir() / "orchestral_techno.wav").string(), chai::math::Vec3{5, 0, 0});

    engine.run([](const UpdateContext&) {});

    engine.shutdown();

    return 0;
}