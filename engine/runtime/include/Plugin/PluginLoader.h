/**
 * @file PluginLoader.h
 * @brief Helper class to load dynamic plugins
 */
#pragma once
#include <Plugin/DynamicLibrary.h>
#include <Plugin/PluginBase.h>
#include <filesystem>
#include <memory>
#include <span>
#include <vector>

namespace chai
{
    using CreatePluginFn = IPlugin* (*)();

    /**
     * @brief Discovers plugin DLLs, loads them. The Loader should outlive the engine
     */
    class PluginLoader
    {
    public:
        PluginLoader() = default;
        explicit PluginLoader(const IPlugin::ServiceList& providedServices) : providedServices_(providedServices) {}

        /**
         * @brief Load one plugin library
         * @return Pointer to the plugin or nullptr on failure
         */
        IPlugin* load(const std::filesystem::path& libPath);

        /**
         * @brief Scan a directory and load every plugin in it.
         * 
         * @return Success
         */
        [[nodiscard]] bool loadDirectory(const std::filesystem::path& dir);

        struct Loaded {
            DynamicLibrary library;
            std::unique_ptr<IPlugin> plugin;
        };
        std::span<const Loaded> loadedPluginInfo() const { return loaded_; }

        std::span<IPlugin* const> plugins() const { return pluginPtrs_; }

    private:
        IPlugin::ServiceList providedServices_;
        std::vector<Loaded> loaded_;
        std::vector<IPlugin*> pluginPtrs_;
    };
} // namespace chai