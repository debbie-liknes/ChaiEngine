#pragma once
#include "CoreExport.h"

#include <Plugin/PluginBase.h>
#include <string>
#include <memory>

namespace chai
{
    //class CORE_EXPORT IAsset
    //{
    //public:
    //    virtual ~IAsset() = default;
    //    virtual bool isValid() const = 0;
    //    virtual const std::string& getAssetId() const = 0;

    //protected:
    //    bool m_valid{false};
    //    std::string m_assetId;
    //};

    class CORE_EXPORT IAssetLoader
    {
    public:
        virtual ~IAssetLoader() = default;
        virtual bool canLoad(const std::string& extension) const = 0;

        // The asset type this loader produces, as a type_index.
        // The plugin sets this; the core uses it for dispatch.
        virtual std::type_index assetType() const = 0;

        // Type-erased load. Returns a void* owning pointer (actually a T*)
        // plus a deleter that knows the real type. The caller casts to T*
        // only after verifying assetType() matches.
        virtual std::unique_ptr<void, void (*)(void*)> loadErased(const std::string& path) = 0;
    };
}