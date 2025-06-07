#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <Plugin/PluginRegistry.h>

namespace chai
{
    class ServiceLocator 
    {
    public:
        static ServiceLocator& instance();

        template<typename T>
        std::shared_ptr<T> get(const std::string& serviceName) {
            // Try to find service across all loaded plugins
            auto& registry = kettle::PluginRegistry::instance();
            for (const auto& [pluginName, plugin] : registry.loadedPlugins_) {
                if (auto service = plugin->getServices().getService<T>(serviceName)) {
                    return service;
                }
            }
            return nullptr;
        }

        template<typename T>
        std::shared_ptr<T> getFrom(const std::string& pluginName, const std::string& serviceName) {
            return kettle::PluginRegistry::instance().getService<T>(pluginName, serviceName);
        }
    };
}