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
#include <Clock.h>

namespace chai
{
    /**
     * @brief Owns the main loop, plugin lifecycle, and overall orchestration. The Engine is the
     * only code that directly interacts with plugins, and it does so through the PluginContext.
     */
    class Engine
    {
    public:
        void run();
        void startup();
        void shutdown();

    private:
        ServiceLocator services_;
        TypeRegistry& types_ = TypeRegistry::instance();
        PluginContext ctx_{services_, types_};
        std::vector<IPlugin*> active_;
        bool running_ = true;

        void mainLoop();
    };
}