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
        /**
         * @brief Load one plugin library
         * @return Pointer to the plugin or nullptr on failure
         */
        IPlugin* load(const std::filesystem::path& libPath);

        /**
         * @brief Scane a directory and load every plugin in it.
         * 
         * @todo Order of loading is unspecified. This may bite me later
         * 
         * @return Count of plugins loaded
         */
        std::size_t loadDirectory(const std::filesystem::path& dir);

        std::span<IPlugin* const> plugins() const { return pluginPtrs_; }

    private:
        struct Loaded {
            DynamicLibrary library;
            std::unique_ptr<IPlugin> plugin;
        };

        std::vector<Loaded> loaded_;
        std::vector<IPlugin*> pluginPtrs_;
    };
} // namespace chai