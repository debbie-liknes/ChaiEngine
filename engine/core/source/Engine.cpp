#include <Engine.h>
#include <Log.h>

namespace chai
{
    void Engine::mainLoop()
    {
        Clock clock;
        while (running_) {
            float dt = clock.tick();
        }
    }

    void Engine::run()
    {
        startup();
        mainLoop();
        shutdown();
    }

    void Engine::startup()
    {
        setLogSink(&logSink_);
        setLogLevel(LogLevel::Info);
        CHAI_LOG_INFO("Engine starting");
        // (logger installed before this point — see below)
        for (auto& p : PluginRegistry::instance().plugins()) {
            p->onLoad(ctx_);
            active_.push_back(p.get());
        }
    }

    void Engine::shutdown()
    {
        CHAI_LOG_INFO("Engine shutdown");
        for (auto it = active_.rbegin(); it != active_.rend(); ++it)
            (*it)->onUnload(ctx_);
    }
}