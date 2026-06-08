/**
 * @file Engine.h
 * @brief The main engine class, responsible for plugin lifecycle and overall orchestration
 */
#pragma once
#include <Plugin/ServiceLocator.h>
#include <Plugin/PluginContext.h>
#include <Plugin/PluginBase.h>
#include <TypeRegistry.h>
#include <string>
#include <vector>
#include <Clock.h>
#include <span>

namespace chai
{
    /**
     * @brief Owns the main loop, plugin lifecycle, and overall orchestration. The Engine is the
     * only code that directly interacts with plugins, and it does so through the PluginContext.
     */
    class Engine
    {
    public:
        //lifecycle methods
        void startup();
        bool tick();
        void shutdown();
        void requestStop();
        ServiceLocator& services() { return services_; }
        void setPlugins(std::span<IPlugin* const> p);

    private:
        ServiceLocator services_;
        std::vector<IPlugin*> plugins_;
        TypeRegistry& types_ = TypeRegistry::instance();
        PluginContext ctx_{services_, types_};
        std::vector<IPlugin*> active_;
        bool running_ = true;
        Clock clock_;
    };
}