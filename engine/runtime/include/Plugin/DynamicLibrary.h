/**
 * @file DynamicLibrary.h
 * @brief RAII wrapper over a loaded shared library
 */
#pragma once
#include <filesystem>

namespace chai
{
    /**
     * @brief RAII wrapper over a loaded shared library
     */
    class DynamicLibrary
    {
    public:
        DynamicLibrary() = default;
        explicit DynamicLibrary(const std::filesystem::path& path);
        ~DynamicLibrary();

        DynamicLibrary(DynamicLibrary&&) noexcept;
        DynamicLibrary& operator=(DynamicLibrary&&) noexcept;
        DynamicLibrary(const DynamicLibrary&) = delete;
        DynamicLibrary& operator=(const DynamicLibrary&) = delete;

        bool valid() const { return handle_ != nullptr; }
        void* symbol(const char* name) const; // nullptr if not found
        void close();

        std::string getName() const { return name_; }
        std::string getAuthor() const { return author_; }
        std::string getVersion() const { return version_; }
        std::string getBuildTimestamp() const { return build_timestamp_; }
        std::string getBinaryPath() const { return binary_; }
        bool isDevelopment() const { return development_; }
        bool isReloadable() const { return reloadable_; }
        std::string getChecksum() const { return checksum_sha256_; }

    private:
        void* handle_ = nullptr;

        // metadata
        std::string name_;
        std::string author_;
        std::string version_;
        std::string build_timestamp_;
        std::string binary_;
        bool development_ = false;
        bool reloadable_ = false;
        std::string checksum_sha256_;
    };
} // namespace chai