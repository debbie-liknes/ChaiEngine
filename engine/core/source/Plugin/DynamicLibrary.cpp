#include <Plugin/DynamicLibrary.h>
#include <Log.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace chai
{
    DynamicLibrary::DynamicLibrary(const std::filesystem::path& path)
    {
#if defined(_WIN32)
        handle_ = ::LoadLibraryW(path.c_str());
        if (!handle_)
            CHAI_LOG_ERROR(
                "LoadLibrary failed for '{}' (error {})", path.string(), ::GetLastError());
#else
        // RTLD_LAZY loads symbols only as they are executed.
        handle_ = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle_)
            CHAI_LOG_ERROR(
                "dlopen failed for '{}' (error {})", path.string(), dlerror());
#endif
    }

    DynamicLibrary::~DynamicLibrary()
    {
        close();
    }

    DynamicLibrary::DynamicLibrary(DynamicLibrary&& o) noexcept : handle_(o.handle_)
    {
        o.handle_ = nullptr;
    }

    DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& o) noexcept
    {
        if (this != &o) {
            close();
            handle_ = o.handle_;
            o.handle_ = nullptr;
        }
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
