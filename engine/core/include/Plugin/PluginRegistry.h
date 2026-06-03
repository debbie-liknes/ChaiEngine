/**
 * @file PluginRegistry.h
 * @brief Defines the PluginRegistry class, which maintains a static inventory of all plugins
 * registered at static-init time
 */
#pragma once
#include <Plugin/PluginBase.h>
#include <memory>
#include <vector>

namespace chai
{
    /**
     * @brief A singleton registry that maintains a static inventory of all plugins registered at
     * static-init time
     */
    class PluginRegistry
    {
    public:
        static PluginRegistry& instance();

        void add(std::unique_ptr<IPlugin> plugin) { plugins_.push_back(std::move(plugin)); }

        const std::vector<std::unique_ptr<IPlugin>>& plugins() const { return plugins_; }

    private:
        std::vector<std::unique_ptr<IPlugin>> plugins_;
    };
} // namespace chai