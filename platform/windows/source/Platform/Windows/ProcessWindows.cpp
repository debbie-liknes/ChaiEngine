#include <Platform/Windows/ProcessWindows.h>

#include <windows.h>

namespace chai::windows
{
    int ProcessWindows::execute()
    {
        STARTUPINFOW startupInfo = {0};
        startupInfo.cb = sizeof(STARTUPINFOW);

        PROCESS_INFORMATION processInfo = {nullptr};

        bool bInheritHandles = true;
        DWORD dwCreationFlags =
            EXTENDED_STARTUPINFO_PRESENT | CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS;
        int ret = CreateProcessW(nullptr, nullptr, nullptr, nullptr, bInheritHandles, dwCreationFlags, nullptr, nullptr,
            &startupInfo, &processInfo);
        return 0;
    }
}

