#include <Log.h>
#include <Plugin/PluginMacros.h>
#include <Plugin/PluginLoader.h>

namespace chai
{
    namespace
    {
#if defined(_WIN32)
        constexpr auto kPluginExt = ".dll";
#else
        constexpr auto kPluginExt = ".so";
#endif
    } // namespace

    IPlugin* PluginLoader::load(const std::filesystem::path& libPath)
    {
        DynamicLibrary lib(libPath);
        if (!lib.valid())
            return nullptr;

        auto abiFn = reinterpret_cast<int (*)()>(lib.symbol("chaiPluginAbiVersion"));
        if (!abiFn) {
            CHAI_LOG_ERROR("'{}': not a chai plugin (no chaiPluginAbiVersion)", libPath.string());
            return nullptr;
        }
        if (const int v = abiFn(); v != CHAI_PLUGIN_ABI_VERSION) {
            CHAI_LOG_ERROR("'{}': plugin ABI {} != engine ABI {}",
                           libPath.string(),
                           v,
                           CHAI_PLUGIN_ABI_VERSION);
            return nullptr;
        }

        auto createFn = reinterpret_cast<CreatePluginFn>(lib.symbol("chaiCreatePlugin"));
        if (!createFn) {
            CHAI_LOG_ERROR("'{}': missing chaiCreatePlugin entry point", libPath.string());
            return nullptr;
        }

        IPlugin* plugin = createFn();
        if (!plugin) {
            CHAI_LOG_ERROR("'{}': chaiCreatePlugin returned null", libPath.string());
            return nullptr;
        }

        CHAI_LOG_INFO("Loaded plugin '{}' from '{}'", plugin->name(), libPath.string());

        loaded_.push_back({std::move(lib), std::unique_ptr<IPlugin>(plugin)});
        pluginPtrs_.push_back(plugin);
        return plugin;
    }

    std::size_t PluginLoader::loadDirectory(const std::filesystem::path& dir)
    {
        namespace fs = std::filesystem;
        std::error_code ec;
        if (!fs::is_directory(dir, ec)) {
            CHAI_LOG_WARN("Plugin directory '{}' not found", dir.string());
            return 0;
        }

        std::size_t count = 0;
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (!entry.is_regular_file())
                continue;
            if (entry.path().extension() != kPluginExt)
                continue;
            if (load(entry.path()))
                ++count;
        }
        CHAI_LOG_INFO("Loaded {} plugin(s) from '{}'", count, dir.string());
        return count;
    }
} // namespace chai
