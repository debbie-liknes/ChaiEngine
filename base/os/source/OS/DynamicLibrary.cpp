#include <OS/DynamicLibrary.h>
#include <Log.h>
#include <json.hpp>
#include <picosha2_fast.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
constexpr auto kPluginExt = ".dll";
#else
#include <dlfcn.h>
constexpr auto kPluginExt = ".so";
#endif

namespace fs = std::filesystem;

namespace chai
{
    [[nodiscard]] std::string computeHash(const std::filesystem::path& binaryPath)
    {
        std::ifstream f(binaryPath, std::ios::binary);
        return picosha2::hash256_hex_string(f);
    }

    DynamicLibrary::DynamicLibrary(const std::filesystem::path& manifestPath)
    {
        if (!fs::exists(manifestPath)) {
            CHAI_LOG_ERROR("Manifest file not found for '{}'", manifestPath.string());
            return;
        }

        using json = nlohmann::json;
        try
        {
            std::ifstream manifestFile(manifestPath);
            json manifest = json::parse(manifestFile);
            if (!manifest.contains("binary")) {
                CHAI_LOG_ERROR("Binary filename missing for '{}'", manifestPath.string());
                return;
            }

            std::string binaryFileName = manifest["binary"];
            auto binaryFile = manifestPath.parent_path() / binaryFileName;
            if (!fs::exists(binaryFile)) {
                CHAI_LOG_ERROR("Binary file not found for '{}'", manifestPath.string());
                return;
            }

            // validate checksum for non-development plugins
            // TODO: this should also verify that the engine itself is not in development
            if (!manifest.contains("development") || manifest["development"] != true) {
                if (!manifest.contains("checksum_sha256")) {
                    CHAI_LOG_ERROR("Checksum data missing for '{}'", manifestPath.string());
                    return;
                }

                if (computeHash(binaryFile) != manifest["checksum_sha256"]) {
                    CHAI_LOG_ERROR("Checksum of '{}' does not match checksum given in '{}'",
                                   binaryFile.string(),
                                   manifestPath.string());
                    return;
                }
            }

            // Look for associated plugin metainfo.
#if defined(_WIN32)
            handle_ = ::LoadLibraryW(binaryFile.c_str());
            if (!handle_)
                CHAI_LOG_ERROR(
                    "LoadLibrary failed for '{}' (error {})", binaryFile.string(), ::GetLastError());
#else
            // RTLD_LAZY loads symbols only as they are executed.
            handle_ = dlopen(binaryFile.c_str(), RTLD_LAZY);
            if (!handle_)
                CHAI_LOG_ERROR("dlopen failed for '{}' (error {})", binaryFile.string(), dlerror());
#endif

            // Set dynamic library metadata after everything has been loaded
            binary_ = binaryFile.string();
            if (manifest.contains("name"))
                name_ = manifest["name"];
            if (manifest.contains("author"))
                author_ = manifest["author"];
            if (manifest.contains("version"))
                version_ = manifest["version"];
            if (manifest.contains("reloadable"))
                reloadable_ = manifest["reloadable"];
            if (manifest.contains("build_timestamp"))
                build_timestamp_ = manifest["build_timestamp"];
            if (manifest.contains("development"))
                development_ = manifest["development"];
            if (manifest.contains("checksum_sha256"))
                checksum_sha256_ = manifest["checksum_sha256"];
        }
        catch (json::parse_error& ex)
        {
            CHAI_LOG_ERROR("{}", ex.what());
        }
    }

    DynamicLibrary::~DynamicLibrary()
    {
        close();
    }

    DynamicLibrary::DynamicLibrary(DynamicLibrary&& o) noexcept : handle_(o.handle_)
    {
        o.handle_ = nullptr;
        name_ = o.name_;
        author_ = o.author_;
        version_ = o.version_;
        build_timestamp_ = o.build_timestamp_;
        binary_ = o.binary_;
        development_ = o.development_;
        reloadable_ = o.reloadable_;
        checksum_sha256_ = o.checksum_sha256_;
    }

    DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& o) noexcept
    {
        if (this != &o) {
            close();
            handle_ = o.handle_;
            o.handle_ = nullptr;
        }
        name_ = o.name_;
        author_ = o.author_;
        version_ = o.version_;
        build_timestamp_ = o.build_timestamp_;
        binary_ = o.binary_;
        development_ = o.development_;
        reloadable_ = o.reloadable_;
        checksum_sha256_ = o.checksum_sha256_;
        return *this;
    }

    void* DynamicLibrary::symbol(const char* name) const
    {
        if (!handle_)
            return nullptr;
#if defined(_WIN32)
        return reinterpret_cast<void*>(::GetProcAddress(static_cast<HMODULE>(handle_), name));
#else
        return dlsym(handle_, name);
#endif
    }

    void DynamicLibrary::close()
    {
        if (!handle_)
            return;
#if defined(_WIN32)
        ::FreeLibrary(static_cast<HMODULE>(handle_));
#else
        dlclose(handle_);
#endif
        handle_ = nullptr;
    }
} // namespace chai
