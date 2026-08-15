#pragma once

namespace chai::windows
{
    class ProcessWindows : public Process
    {
    public:
        explicit ProcessWindows(const Process::ProcessInfo& info)
            : Process(info) {}

        
    };
}

