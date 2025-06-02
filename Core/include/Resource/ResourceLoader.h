#pragma once
#include "CoreExport.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace chai
{
    class CORE_EXPORT IResource {
    public:
        virtual ~IResource() = default;
        virtual const std::string GetName() const = 0;
    };

    class CORE_EXPORT IResourceLoader {
    public:
        virtual ~IResourceLoader() = default;
        virtual bool CanLoad(const std::string& extension) const = 0;
        virtual std::shared_ptr<IResource> Load(const std::string& path) = 0;
    };
}