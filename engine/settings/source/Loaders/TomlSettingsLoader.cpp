#include <Loaders/TomlSettingsLoader.h>

#include <Log.h>

namespace chai::settings
{
    bool TomlSettingsLoader::canLoad(std::string_view ext) const
    {
        return ext == ".toml";
    }

    const char* TomlSettingsLoader::name() const
    {
        return "TomlSettingsLoader";
    }

    std::optional<SettingsAsset> TomlSettingsLoader::decode(std::span<const uint8_t> bytes,
        const std::filesystem::path& baseDir)
    {
        try
        {
            // Cast the byte pointers to char* to construct a string_view
            std::string_view view{reinterpret_cast<const char*>(bytes.data()), bytes.size()};

            toml::table tbl = toml::parse(view);
            std::stringstream ss;
            ss << tbl;
            CHAI_LOG_INFO("{}", ss.str());

            SettingsAsset asset;

            return asset;

        }
        catch (const toml::parse_error& err)
        {
            CHAI_LOG_ERROR("Parsing failed:\n{}\n", err.what());
            return std::optional<SettingsAsset>();
        }
    }

} // namespace chai::settings
