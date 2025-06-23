#include <Plugin/PluginRegistry.h>
#include <stdexcept>
#include <filesystem>
#include <iostream>

namespace chai::kettle
{
    std::filesystem::path get_executable_directory() {
#ifdef _WIN32
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        return std::filesystem::path(path).parent_path();
#elif defined(__linux__)
        char path[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
        return std::filesystem::path(std::string(path, count)).parent_path();
#endif
    }

    PluginRegistry& PluginRegistry::instance() 
    {
        static PluginRegistry reg;
        return reg;
    }

    static void printLoadedPlugin(std::shared_ptr<IPlugin> plugin)
    {
        std::cout << "Loaded plugin: " << plugin->getName() << " v" << plugin->getVersion() << "\n";
        std::cout << " - Services:\n";
        for (auto& name : plugin->getServices().getServiceNames())
        {
            std::cout << "   - " << name << "\n";
        }
    }

    // Register a factory under a category and name
    void PluginRegistry::registerPlugin(const std::string& pluginName, std::function<std::unique_ptr<IPlugin>()> factory)
    {
        plugins_[pluginName] = factory;
    }

    bool PluginRegistry::loadLibrary(const std::string& path)
    {
#ifdef _WIN32
        PluginHandle handle = LoadLibraryA(path.c_str());
#else
        PluginHandle handle = dlopen(path.c_str(), RTLD_LAZY);
#endif

        if (!handle) {
            std::cerr << "Failed to load plugin: " << path << "\n";
            return false;
        }

        // GetPluginManifest signature
        using RegisterPluginFn = void** (*)();

#ifdef _WIN32
        auto registerHandle = (RegisterPluginFn)GetProcAddress(handle, "RegisterPlugin");
#else
        auto getPlugin = (RegisterPluginFn)dlsym(handle, "GetPluginModule");
#endif

        if (!registerHandle) {
            std::cerr << "Plugin " << path << " does not expose RegisterPlugin()\n";
            return false;
        }

        registerHandle();

        m_plugins.push_back({ handle, path });
        return true;
    }

    std::shared_ptr<IPlugin> PluginRegistry::loadPlugin(const std::string& pluginPath)
    {
		std::string pluginName = std::filesystem::path(pluginPath).filename().stem().string();
        auto it = plugins_.find(pluginName);
        if (it != plugins_.end()) 
        {
            return loadedPlugins_[pluginName];
        }

        //load it and try again
        if (loadLibrary(pluginPath))
        {
            it = plugins_.find(pluginName);
            if (it != plugins_.end()) 
            {
                auto plugin = std::shared_ptr<IPlugin>(it->second().release());
                plugin->initialize();
                printLoadedPlugin(plugin);
                loadedPlugins_[pluginName] = plugin;
                return plugin;
            }
        }

        return nullptr;
    }

    bool PluginRegistry::loadPluginsInDirectory(const std::string& directory)
    {
        auto exePath = get_executable_directory();
        std::filesystem::path pluginsPath = exePath.append(directory);
        if (!std::filesystem::exists(pluginsPath)) {
            std::cerr << "Plugin directory does not exist: " << pluginsPath << "\n";
            return false;
        }
        for (const auto& entry : std::filesystem::directory_iterator(pluginsPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".dll") {
                if (!loadPlugin(entry.path().string())) {
                    std::cerr << "Failed to load plugin: " << entry.path() << "\n";
                }
            }
        }
        return true;
    }
}