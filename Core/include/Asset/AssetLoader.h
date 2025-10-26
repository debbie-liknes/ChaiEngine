#pragma once
#include "CoreExport.h"

#include <Plugin/PluginBase.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <typeindex>

namespace chai
{
    class CORE_EXPORT IAsset
    {
    public:
        virtual ~IAsset() = default;
        virtual bool isValid() const = 0;
        virtual const std::string& getAssetId() const = 0;
    };

    class CORE_EXPORT IAssetLoader
    {
    public:
        virtual ~IAssetLoader() = default;
        virtual bool canLoad(const std::string& extension) const = 0;
        virtual std::shared_ptr<IAsset> load(const std::string& path) = 0;
    };
}