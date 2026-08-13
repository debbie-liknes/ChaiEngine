/**
 * @file PluginBase.h
 * @brief Defines the IPlugin interface that all plugins must implement
 */
#pragma once
#include <RuntimeExport.h>
#include <string>
#include <unordered_set>
#include <typeindex>
#include <Plugin/PluginContext.h>

namespace chai
{
    struct PluginContext;
    /**
     * @brief The base interface for all plugins. Each plugin must implement this interface and
     * provide a unique name, as well as onLoad and onUnload methods to manage their lifecycle. The
     * Engine calls onLoad when the plugin is loaded, allowing it to register services, types, and
     * systems, and calls onUnload when the plugin is unloaded, allowing it to clean up in reverse
     * order.
     */
    class IPlugin
    {
    public:
        virtual ~IPlugin() = default;
        virtual const char* name() const = 0;
        using ServiceList = std::unordered_set<std::type_index>;
        virtual ServiceList providedServices() const = 0;
        virtual ServiceList requiredServices() const = 0;
        virtual void onLoad(PluginContext& ctx) = 0;
        virtual void onUnload(PluginContext& ctx) = 0;
    };
}