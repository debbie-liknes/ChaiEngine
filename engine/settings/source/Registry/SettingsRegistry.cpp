#include <Registry/SettingsRegistry.h>

#include <Loaders/TomlSettingsLoader.h>
#include <Core/FileIO.h>
#include <Log.h>

namespace chai::settings
{
    void SettingsRegistry::load(AssetId id, const std::filesystem::path& path)
    {
        TomlSettingsLoader loader;
        if (loader.canLoad(path.extension().string()))
        {
            auto bytes = readFileBytes(path);
            if (bytes.empty()) {
                CHAI_LOG_ERROR("SettingsRegistry::load: could not read '{}'", path.string());
                return;
            }

            auto parsed = loader.decode(bytes, path.parent_path());
            if (!parsed) {
                CHAI_LOG_ERROR("SettingsRegistry::load: decode failed for '{}'", path.string());
                return;
            }
        }
    }

    void SettingsRegistry::release(AssetId id)
    {

    }

} // namespace chai::settings
