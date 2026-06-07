#include <Engine.h>
#include <Log.h>

namespace chai
{
    void Engine::startup()
    {
        CHAI_LOG_INFO("Engine starting");
        for (auto& p : PluginRegistry::instance().plugins()) {
            p->onLoad(ctx_);
            active_.push_back(p.get());
        }
    }

    bool Engine::tick()
    {
        CHAI_LOG_INFO("Tick");
        float dt = clock_.tick();
        (void)dt;
        return running_;
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
}