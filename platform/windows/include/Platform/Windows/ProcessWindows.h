#pragma once

#include <OS/Process.h>

namespace chai::windows
{
    class ProcessWindows : public Process
    {
    public:
        explicit ProcessWindows(const Process::Info& info) : info_(info) {}

        virtual int execute() final;

    private:
        Process::Info info_;
    };
}

