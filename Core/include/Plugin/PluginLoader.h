#pragma once
#include <CoreExport.h>
#include <iostream>
#include <vector>
#include <Plugin/PluginManifest.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace chai
{
#ifdef _WIN32
#include <Windows.h>
    using PluginHandle = HMODULE;
#else
#include <dlfcn.h>
    using PluginHandle = void*;
#endif

    struct CORE_EXPORT LoadedPlugin {
        PluginHandle handle;
        PluginManifest* manifest;
        std::string path;
    };

    class CORE_EXPORT PluginLoader {
    public:
        bool LoadPlugin(const std::string& path);
        const std::vector<LoadedPlugin>& GetPlugins() const { return m_plugins; }

    private:
        std::vector<LoadedPlugin> m_plugins;
    };
}