#include <Platform/Windows/ProcessWindows.h>
#include <OS/SystemPaths.h>

#include <Windows.h>

namespace
{
    constexpr const char* APP_NAME = "ChaiGame.exe";
}

namespace chai::windows
{
    int ProcessWindows::execute()
    {
        STARTUPINFO startupInfo = {sizeof(STARTUPINFO)};
		PROCESS_INFORMATION processInfo = {nullptr};

		// Create a mutable command line buffer
        std::string commandLine = APP_NAME;
        for (const auto& arg : info_.args)
            commandLine += " " + arg;

		// Use standard creation flags (removed EXTENDED_STARTUPINFO_PRESENT)
		DWORD dwCreationFlags = CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS;

		BOOL success =
			CreateProcessA(nullptr, // Application Name (optional if in commandLine)
						   commandLine.data(), // Command Line string (LPWSTR)
						   nullptr,            // Process Attributes
						   nullptr,            // Thread Attributes
						   true,               // Inherit Handles
						   dwCreationFlags,    // Creation Flags
						   nullptr,            // Environment
						   nullptr,            // Current Directory
						   &startupInfo,
						   &processInfo);

		if (success) {
			// Always clean up open handles if you aren't storing them
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
		} else {
			// Retrieve Win32 Error Code (e.g., GetLastError())
			DWORD err = GetLastError();
		}

		return success ? 0 : -1;
	}
}

