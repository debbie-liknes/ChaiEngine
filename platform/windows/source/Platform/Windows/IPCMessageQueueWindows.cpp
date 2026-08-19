#include <Platform/Windows/IPCMessageQueueWindows.h>

#include <Windows.h>

#include <Log.h>
#include <string>
#include <memory>

namespace
{
    HANDLE mutex_g = nullptr;
    HANDLE fileMappingHandle_g = nullptr;
    LPVOID fileMappingView_g = nullptr;
}

namespace chai::windows
{
    IPCMessageQueueWindows::IPCMessageQueueWindows() : IPCMessageQueue()
    {
        SECURITY_ATTRIBUTES sa = {0};
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = true;
        sa.lpSecurityDescriptor = nullptr;

        mutex_g = CreateMutexA(&sa, false, nullptr);
        if (!mutex_g)
            return;

        WaitForSingleObject(mutex_g, INFINITE);

        // INVALID_HANDLE_VALUE ensure this uses the system page file to create a shared memory page
        fileMappingHandle_g = CreateFileMappingA(INVALID_HANDLE_VALUE,   // hFile
                                                 &sa,                    // lpFileMappingAttributes
                                                 PAGE_EXECUTE_READWRITE, // flProtect
                                                 0x0,                    // dwMaximumSizeLow
                                                 sizeof(decltype(buffer_)) + 24,               // dwMaximumSizeHigh
                                                 nullptr);     // lpName
        if (!fileMappingHandle_g) {
            ReleaseMutex(mutex_g);
            return;
        }

        isOwningProcess_ = (GetLastError() != ERROR_ALREADY_EXISTS);

        fileMappingView_g =
            MapViewOfFile(fileMappingHandle_g, FILE_MAP_ALL_ACCESS, 0x0, 0x0, sizeof(decltype(buffer_)) + 24);
        buffer_ = std::bit_cast<decltype(buffer_)>(fileMappingView_g);

        if (buffer_) {
            if (isOwningProcess_) {
                // We're responsible for constructing the ring buffer
                std::construct_at(buffer_);
                                CHAI_LOG_INFO("Creating new ring buffer mapping.");
            } else {
                // while (InterlockedCompareExchange(&buffer_->initialized, 1, 1) != 1) {
                //     Sleep(1);
                // }
                CHAI_LOG_INFO("Joining existing ring buffer mapping.");
            }
        }

        ReleaseMutex(mutex_g);
    }

    IPCMessageQueueWindows::~IPCMessageQueueWindows()
    {
        if (buffer_) {
            std::destroy_at(buffer_);
        }

        if (fileMappingView_g != nullptr) {
            UnmapViewOfFile(fileMappingView_g);
            fileMappingView_g = nullptr; // Prevent dangling pointer
        }

        // Step 2: Close the file mapping handle
        if (fileMappingHandle_g != nullptr && fileMappingHandle_g != INVALID_HANDLE_VALUE) {
            CloseHandle(fileMappingHandle_g);
            fileMappingHandle_g = nullptr;
        }
    }

    void IPCMessageQueueWindows::processPendingMessages()
    {
        // Only the client process should handle pending messages
        if (isOwningProcess_ || !buffer_)
            return;

        while (!buffer_->empty())
        {
            auto message = buffer_->pop();

            if (!message.has_value())
                continue;

            const auto& value = message.value();

            switch(value.type)
            {
                case IPCMessageType::PLAY:
                    CHAI_LOG_INFO("Received PLAY message from host");
                    break;
                case IPCMessageType::PAUSE:
                    CHAI_LOG_INFO("Received PAUSE message from host");
                    break;
                default:
                    // TODO: This should be a string "pretty" value when enum conversion
                    // is available.
                    CHAI_LOG_WARN("Received message of unknown type {}", std::to_string((int)value.type));
                    break;
            }
        }
    }
	
    void IPCMessageQueueWindows::sendIPCMessage(const IPCMessage& message)
    {
        // Only the owning process should send IPC messages
        if (!isOwningProcess_ || !buffer_)
            return;

        
        CHAI_LOG_INFO("Sending PLAY message to client");
        buffer_->push(message);
    }

} // namespace chai::windows
