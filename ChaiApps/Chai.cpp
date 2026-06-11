#include <Engine.h>
#include <SpdLogSink.h>
#include <Log.h>
#include <Window/Window.h>
#include <Plugin/PluginLoader.h>
#include <SystemPaths.h>
#include <Renderer.h>
#include <Primitives.h>
#include <MeshAsset.h>

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

    static auto start = std::chrono::high_resolution_clock::now();

    // once, at startup:
    auto registry = engine.services().tryResolve<gfx::IMeshRegistry>();
    Handle<gfx::Mesh> cube = registry->ingest(makeAssetId("builtin:cube"), gfx::makeCube(1.0f));
    if (!cube.valid())
    {
        CHAI_LOG_ERROR("Cube is invalid");
    }

    //main loop
    while (!win->shouldClose()) {
        auto events = win->pollEvents();

        //temp
        auto now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float>(now - start).count(); // seconds since start
        float angle = time * math::radians(90.0f); // 90°/second, regardless of fps

        int w = 0, h = 0;
        win->framebufferSize(w, h);

        gfx::FrameRenderData frame;
        float aspect = 1.f * w / h;
        frame.view = math::lookAt(math::Vec3{0, 0, 3}, math::Vec3{0, 0, 0}, math::Vec3{0, 1, 0});
        frame.proj = math::perspectiveVK(math::radians(60.f), aspect, 0.1f, 100.f);

        math::Quaternion q = math::Quat::fromAxisAngle(math::Vec3{0, 1, 0}, angle);
        math::Mat4 model = q.toMat4();

        frame.objects.emplace_back(cube, model);

        renderer->renderFrame(frame);
        engine.tick();
    }

    renderer.reset();
    win.reset();

    engine.shutdown();
}