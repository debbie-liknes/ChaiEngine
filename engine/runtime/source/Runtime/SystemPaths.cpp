#include <Log.h>
#include <Runtime/SystemPaths.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
// Linux: resolve the /proc/self/exe symlink.
#endif

namespace chai
{
    std::filesystem::path executableDir()
    {
        std::error_code ec;

#if defined(_WIN32)
        std::wstring buf(MAX_PATH, L'\0');
        for (;;) {
            const DWORD len =
                ::GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
            if (len == 0) {
                CHAI_LOG_ERROR("GetModuleFileNameW failed (error {})", ::GetLastError());
                return {};
            }
            if (len < buf.size())
            {
                buf.resize(len);
                break;
            }
            buf.resize(buf.size() * 2);
        }
        return std::filesystem::path(buf).parent_path();

#else // Linux and other /proc systems
        try {
            // /proc/self/exe is a symlink to the currently running executable
            std::filesystem::path exePath = std::filesystem::read_symlink("/proc/self/exe");
            
            // Return just the parent directory path
            return exePath.parent_path();
        } 
        catch (const std::filesystem::filesystem_error& e) {
            CHAI_LOG_ERROR("Filesystem error: {}", e.what());
            return "";
        }
#endif
    }
} // namespace chai
