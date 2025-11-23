#pragma once
#include <Plugin/PluginBase.h>
#include <functional>
#include <string>
#include <memory>
#include <Types/CMap.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace chai::kettle
{
    using FactoryFn = std::function<void*()>;

#ifdef _WIN32
    using PluginHandle = HMODULE;
#else
    using PluginHandle = void*;
#endif

    struct LoadedPlugin
    {
        PluginHandle handle;
        std::string path;
    };

    class PluginRegistry
    {
    public:
        PluginRegistry() = default;
        static PluginRegistry& instance();

        void registerPlugin(const std::string& pluginName, std::function<std::unique_ptr<IPlugin>()> factory);

        std::shared_ptr<IPlugin> loadPlugin(const std::string& pluginName);
        bool loadPluginsInDirectory(const std::string& directory);

        std::shared_ptr<IPlugin> getLoadedPlugin(const std::string& pluginName)
        {
            auto it = loadedPlugins_.find(pluginName);
            return (it != loadedPlugins_.end()) ? it->second : nullptr;
        }

        CMap<std::string, std::shared_ptr<IPlugin>> getLoadedPlugins() const
        {
            return loadedPlugins_;
        }

        template <typename T>
        std::shared_ptr<T> getService(const std::string& pluginName, const std::string& serviceName)
        {
            auto plugin = getLoadedPlugin(pluginName);
            if (plugin)
            {
                return plugin->getServices().getService<T>(serviceName);
            }
            return nullptr;
        }

    private:
        CMap<std::string, std::function < std::shared_ptr<IPlugin>()>
        >
        plugins_;
        CMap<std::string, std::shared_ptr<IPlugin>> loadedPlugins_;

        std::vector<LoadedPlugin> m_plugins;

        bool loadLibrary(const std::string& path);
    };
}