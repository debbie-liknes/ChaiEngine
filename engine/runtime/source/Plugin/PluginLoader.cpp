#include <Log.h>
#include <Plugin/PluginMacros.h>
#include <Plugin/PluginLoader.h>
#include <json.hpp>

#include <ranges>

#include <Graph/Algorithms.h>

namespace fs = std::filesystem;

namespace chai
{
    namespace
    {
        constexpr auto kManifestExt = ".json";
    } // namespace
    
    graph::Graph<IPlugin*> buildPluginGraph(std::vector<IPlugin*>& plugins,
                                            const IPlugin::ServiceList& providedServices);

    IPlugin* PluginLoader::load(const fs::path& manifestPath)
    {
        DynamicLibrary lib(manifestPath);
        if (!lib.valid())
            return nullptr;

        auto abiFn = static_cast<int (*)()>(lib.symbol("chaiPluginAbiVersion"));
        if (!abiFn) {
            CHAI_LOG_ERROR("'{}': not a chai plugin (no chaiPluginAbiVersion)",
                           lib.getBinaryPath());
            return nullptr;
        }
        if (const int v = abiFn(); v != CHAI_PLUGIN_ABI_VERSION) {
            CHAI_LOG_ERROR("'{}': plugin ABI {} != engine ABI {}",
                           lib.getBinaryPath(),
                           v,
                           CHAI_PLUGIN_ABI_VERSION);
            return nullptr;
        }

        auto createFn = static_cast<CreatePluginFn>(lib.symbol("chaiCreatePlugin"));
        if (!createFn) {
            CHAI_LOG_ERROR("'{}': missing chaiCreatePlugin entry point", lib.getBinaryPath());
            return nullptr;
        }

        IPlugin* plugin = createFn();
        if (!plugin) {
            CHAI_LOG_ERROR("'{}': chaiCreatePlugin returned null", lib.getBinaryPath());
            return nullptr;
        }

        CHAI_LOG_INFO("Loaded plugin '{}' from '{}'", plugin->name(), lib.getBinaryPath());

        loaded_.emplace_back(std::move(lib), std::unique_ptr<IPlugin>(plugin));
        pluginPtrs_.push_back(plugin);
        return plugin;
    }

    bool PluginLoader::loadDirectory(const std::filesystem::path& dir)
    {
        namespace fs = std::filesystem;
        if (std::error_code ec; !fs::is_directory(dir, ec)) {
            CHAI_LOG_WARN("Plugin directory '{}' not found", dir.string());
            return 0;
        }

        std::size_t count = 0;
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (!entry.is_regular_file())
                continue;
            if (entry.path().extension() != kManifestExt)
                continue;
            if (load(entry.path()))
                ++count;
        }

        auto pluginGraph = buildPluginGraph(pluginPtrs_, providedServices_);

        graph::NodeList<uint32_t> outNodeList;
        if (!graph::topologicalSort(pluginGraph, outNodeList)) {
            CHAI_LOG_CRITICAL(
                "PluginLoader: Plugin dependency cycle detected. Some plugin depends on itself "
                "indirectly.");
            return false;
        }

        std::vector<IPlugin*> sortedPlugins;
        sortedPlugins.reserve(pluginPtrs_.size());

        // Reverse so it is ordered from most-depended-on first
        for (auto idx : outNodeList | std::views::reverse)
            sortedPlugins.push_back(pluginPtrs_[idx]);

        pluginPtrs_ = std::move(sortedPlugins);

        CHAI_LOG_INFO("Loaded {} plugin(s) from '{}'", count, dir.string());
        return true;
    }

    graph::Graph<IPlugin*> buildPluginGraph(std::vector<IPlugin*>& plugins,
                                            const IPlugin::ServiceList& providedServices)
    {
        graph::Graph<IPlugin*> pluginGraph;

        std::unordered_map<std::type_index, uint32_t> service2Plugin;

        for (uint32_t i = 0; i < plugins.size(); i++) {
            pluginGraph.nodes.push_back(plugins[i]);
            for (const auto& service : plugins[i]->providedServices())
                service2Plugin.insert_or_assign(service, i);
        }

        for (uint32_t i = 0; i < plugins.size(); i++) {
            for (const auto& service : plugins[i]->requiredServices()) {
                if (auto itr = service2Plugin.find(service); itr != service2Plugin.end()) {
                    pluginGraph.edges.add(i, itr->second);
                } else if (!providedServices.contains(service)) {
                    CHAI_LOG_CRITICAL("Unresolved service dependency for plugin {}",
                                      plugins[i]->name());
                }
            }
        }

        return pluginGraph;
    }
} // namespace chai
