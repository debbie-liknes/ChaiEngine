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

    private:
        void* handle_ = nullptr;
    };
} // namespace chai