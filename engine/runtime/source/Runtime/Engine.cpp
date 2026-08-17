#include <Runtime/Engine.h>
#include <Log.h>
#include <Window/Window.h>
#include <Scene/Scene.h>
#include <Input/IInput.h>
#include <Audio/IAudioEngine.h>
#include <Visitors/AudioSceneVisitor.h>
#include <Visitors/FrameRenderVisitor.h>
#include <Visitors/SceneSaveVisitor.h>
#include <tracy/Tracy.hpp>

namespace chai
{
    bool Engine::startup()
    {
        //Load plugins
        CHAI_LOG_INFO("Engine starting");
        for (const auto& plugin : plugins_) {
            if (!plugin->onLoad(ctx_)) {
                CHAI_LOG_CRITICAL("Plugin {} failed to load. Exiting prematurely.", plugin->name());
                return false;
            }
            active_.push_back(plugin);
        }

        //Create scene
        scene_ = std::make_unique<scene::Scene>();

        return true;
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

    void Engine::run(const std::function<void(const UpdateContext&)>& updateCallback)
    {
        //These come from plugins, check that they exist
        auto window = services_.tryResolve<IWindow>();
        if (!window) {
            CHAI_LOG_CRITICAL("Could not locate Window Service.");
        }

        auto renderer = services_.tryResolve<gfx::IRenderer>();
        if (!renderer) {
            CHAI_LOG_CRITICAL("Could not locate Renderer Service.");
        }

        auto audio = services_.tryResolve<audio::IAudioEngine>();
        if (!audio) {
            CHAI_LOG_WARN("Could not locate Audio Service. Audio capability will be disabled.");
        }

        auto input = services_.tryResolve<IInput>();
        if (!input) {
            CHAI_LOG_CRITICAL("Could not locate Input Service.");
        }

        scene::AudioSceneVisitor audioVisitor;
        scene::FrameRenderVisitor frameVisitor;

        // The main guts of the application
        while (!window->shouldClose()) {
            FrameMarkStart("Engine");

            input->newFrame();  //tell input to clear deltas FIRST

            std::span<const WindowEvent> events{ window->pollEvents() };

            for (const WindowEvent& event : events) {

                // tell the renderer about resized events
                if (event.type == WindowEventType::Resized)
                    renderer->onResize(event.width, event.height);
            }

            renderer->startFrame();
            float dt = clock_.tick();

            UpdateContext ctx{dt, *input};
            scene_->update(ctx);

            scene_->accept(&audioVisitor);
            scene_->accept(&frameVisitor);

            if (audio) {
                audio->set3dListenersAndOrientations(audioVisitor.getData());
                audio->update();
            }

            updateCallback(ctx);

            renderer->renderFrame(frameVisitor.getData());
            renderer->endFrame();

            audioVisitor.reset();
            frameVisitor.reset();

            FrameMarkEnd("Engine");
        }

        scene::SceneSaveVisitor ssv{};
        if (scene_)
            scene_->accept(&ssv);

    }
}
