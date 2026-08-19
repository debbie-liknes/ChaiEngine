#pragma once

#include <any>

namespace chai
{
    enum class IPCMessageType
    {
        UNKNOWN = 0,
        PLAY = 1,
        PAUSE = 2,
        STOP = 3,
    };

    struct IPCMessage
    {
        IPCMessageType type;
        //std::variant<uint8_t, int, Vec3> data;
    };

    class IPCMessageQueue
    {
    public:
        virtual ~IPCMessageQueue() = default;

        virtual bool isOwningProcess() const = 0;

        virtual void processPendingMessages() = 0;

        virtual void sendIPCMessage(const IPCMessage& message) = 0;
    };
} // namespace chai
