#include <Core/Engine.h>
#include <Log.h>
#include <Window/Window.h>
#include <Rendering/FrameRenderData.h>
#include <Core/IInput.h>

namespace chai
{
    Engine::~Engine()
    {
    }

    void Engine::startup()
    {
        CHAI_LOG_INFO("Engine starting");
        for (auto& p : plugins_) {
            p->onLoad(ctx_);
            active_.push_back(p);
        }
    }

    void Engine::shutdown()
    {
        CHAI_LOG_INFO("Engine shutdown");
        for (auto it = active_.rbegin(); it != active_.rend(); ++it)
            (*it)->onUnload(ctx_);
    }

    void Engine::requestStop()
    {
        running_ = false;
    }

    void Engine::setPlugins(std::span<IPlugin* const> p)
    {
        plugins_.assign(p.begin(), p.end());
    }

    void Engine::setScene(std::unique_ptr<IScene> scene)
    {
        scene_ = std::move(scene);
    }

    void Engine::run()
    {
        auto window = services_.tryResolve<IWindow>();
        if (!window) {
            CHAI_LOG_CRITICAL("Could not locate Window Service.");
        }

        auto renderer = services_.tryResolve<gfx::IRenderer>();
        if (!renderer) {
            CHAI_LOG_CRITICAL("Could not locate Window Service.");
        }

        auto input = services_.tryResolve<IInput>();
        if (!input) {
            CHAI_LOG_CRITICAL("Could not locate Input Service.");
        }

        // The main guts of the application
        while (!window->shouldClose()) {
            input->newFrame();  //tell input to clear deltas FIRST
            window->pollEvents();
            float dt = clock_.tick();

            updateActiveCameraAspect();
            UpdateContext ctx{dt, *input};
            scene_->update(ctx);

            gfx::FrameRenderData frame;
            scene_->extract(frame);
            renderer->renderFrame(frame);
        }
    }

    void Engine::updateActiveCameraAspect()
    {
        auto window = services().tryResolve<IWindow>();
        if (!window || !scene_)
            return; // headless?

        //get framebuffer size
        int w = 0, h = 0;
        window->framebufferSize(w, h);
        if (w == 0 || h == 0)
            return; // minimized

        float aspect = float(w) / float(h);
        scene_->setCameraAspect(aspect);
    }
}