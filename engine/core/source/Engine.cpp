#include <Engine.h>
#include <Log.h>

namespace chai
{
    void Engine::startup()
    {
        CHAI_LOG_INFO("Engine starting");
        for (auto& p : plugins_) {
            p->onLoad(ctx_);
            active_.push_back(p);
        }
    }

    bool Engine::tick()
    {
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

    void Engine::setPlugins(std::span<IPlugin* const> p)
    {
        plugins_.assign(p.begin(), p.end());
    }
}