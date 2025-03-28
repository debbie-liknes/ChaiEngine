#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace chai
{
    class IResource {
    public:
        virtual ~IResource() = default;
        virtual const std::string& GetName() const = 0;
    };

    class IResourceLoader {
    public:
        virtual ~IResourceLoader() = default;
        virtual bool CanLoad(const std::string& extension) const = 0;
        virtual std::shared_ptr<IResource> Load(const std::string& path) = 0;
    };
}