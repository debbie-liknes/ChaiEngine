/**
 * @file Engine.h
 * @brief The main engine class, responsible for plugin lifecycle and overall orchestration
 */
#pragma once
#include <Plugin/ServiceLocator.h>
#include <Plugin/PluginContext.h>
#include <Plugin/PluginRegistry.h>
#include <Plugin/PluginBase.h>
#include <TypeRegistry.h>
#include <string>
#include <vector>

namespace chai
{
    /**
     * @brief Owns the main loop, plugin lifecycle, and overall orchestration. The Engine is the
     * only code that directly interacts with plugins, and it does so through the PluginContext.
     */
    class Engine
    {
    public:
        void startup()
        {
            for (auto& p : PluginRegistry::instance().plugins()) {
                p->onLoad(ctx_); // plugin sees ctx_, never *this
                active_.push_back(p.get());
            }
            // ... dynamically loaded plugins get onLoad'd here too
        }

        void shutdown()
        {
            for (auto it = active_.rbegin(); it != active_.rend(); ++it)
                (*it)->onUnload(ctx_); // reverse order
            active_.clear();
        }

    private:
         
        ServiceLocator services_;
        TypeRegistry& types_ = TypeRegistry::instance();
        PluginContext ctx_{services_, types_}; // the narrowed view

        std::vector<IPlugin*> active_; // load order, for reverse unload
    };
}