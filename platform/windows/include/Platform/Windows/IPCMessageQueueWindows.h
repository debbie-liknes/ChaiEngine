#pragma once

#include <Containers/RingBuffer.h>
#include <OS/IPCMessageQueue.h>

namespace chai::windows
{
    class IPCMessageQueueWindows : public IPCMessageQueue
    {
    public:
        IPCMessageQueueWindows();
        virtual ~IPCMessageQueueWindows();

        // explicitly disallow copy / move
        IPCMessageQueueWindows(const IPCMessageQueueWindows&) = delete;
        IPCMessageQueueWindows& operator=(const IPCMessageQueueWindows&) = delete;
        IPCMessageQueueWindows(IPCMessageQueueWindows&&) = delete;
        IPCMessageQueueWindows& operator=(IPCMessageQueueWindows&&) = delete;

        bool isOwningProcess() const override { return isOwningProcess_; }

        void processPendingMessages() override;

        void sendIPCMessage(const IPCMessage& message) override;
   private:
        RingBuffer<IPCMessage, 256>* buffer_ = nullptr;

        bool isOwningProcess_ = false;
    };
}
