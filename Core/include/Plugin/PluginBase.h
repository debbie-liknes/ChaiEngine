#pragma once
#include <CoreExport.h>
#include <Plugin/ServiceRegistry.h>
#include <string>

namespace chai
{
    class CORE_EXPORT IPlugin 
    {
    public:
        virtual ~IPlugin() = default;
        virtual void initialize() = 0;
        virtual void shutdown() = 0;
        virtual const std::string& getName() const = 0;
        virtual const std::string& getVersion() const = 0;
        virtual ServiceRegistry& getServices() = 0;
    };
}