#pragma once
#include <CoreExport.h>
#include <string>

namespace chai
{
    class IPluginBase {
    public:
        virtual ~IPluginBase() = default;
        virtual const std::string& GetName() const = 0;
        virtual void OnStartup() = 0;
        virtual void OnShutdown() = 0;

        virtual void* GetService(const std::string& category, const std::string& name) { return nullptr; }
    };
}