#include <Core/PluginLoader.h>
#include <Core/PluginBase.h>

namespace chai
{
    bool PluginLoader::LoadPlugin(const std::string& path) {
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
        using GetManifestFn = PluginManifest * (*)();
        using GetPluginBaseFn = IPluginBase * (*)();

#ifdef _WIN32
        auto getManifest = (GetManifestFn)GetProcAddress(handle, "GetPluginManifest");
        auto getPlugin = (GetPluginBaseFn)GetProcAddress(handle, "GetPluginModule");
#else
        auto getManifest = (GetManifestFn)dlsym(handle, "GetPluginManifest");
        auto getPlugin = (GetPluginBaseFn)dlsym(handle, "GetPluginModule");
#endif

        if (!getManifest) {
            std::cerr << "Plugin " << path << " does not expose GetPluginManifest()\n";
            return false;
        }

        if (!getPlugin) {
            std::cerr << "Plugin " << path << " does not expose GetPluginModule()\n";
            return false;
        }

        PluginManifest* manifest = getManifest();
        IPluginBase* plugin = getPlugin();

        std::cout << "Loaded plugin: " << manifest->name << " v" << manifest->version << " by " << manifest->author << "\n";
        for (const auto& type : manifest->registeredTypes) {
            std::cout << " - Reflected type: " << type << "\n";
        }

        if (manifest->onRegister) {
            manifest->onRegister();
        }

        if (plugin){
            plugin->OnStartup();
        }

        m_plugins.push_back({ handle, manifest, path });
        return true;
    }
}