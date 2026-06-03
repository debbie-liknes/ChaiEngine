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

        template <typename InterfaceType>
        std::shared_ptr<InterfaceType> get(const std::string& serviceName = "")
        {
            std::string name = serviceName.empty() ? getDefaultServiceName<InterfaceType>() : serviceName;

            // Try to find service across all loaded plugins
            auto const& registry = kettle::PluginRegistry::instance();
            for (const auto& [pluginName, plugin] : registry.getLoadedPlugins())
            {
                if (auto service = plugin->getServices().getService<InterfaceType>(name))
                {
                    return service;
                }
            }
            return nullptr;
        }

    private:
        template <typename T>
        std::string getDefaultServiceName() const
        {
            // Convert interface type to default service name
            std::string typeName = typeid(T).name();
            // Simple conversion: remove namespace and "System" suffix if present
            if (size_t pos = typeName.find_last_of(':'); pos != std::string::npos)
            {
                typeName = typeName.substr(pos + 1);
            }
            return typeName;
        }
    };
}