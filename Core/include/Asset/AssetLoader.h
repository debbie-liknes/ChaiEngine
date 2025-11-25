#pragma once
#include "CoreExport.h"

#include <Plugin/PluginBase.h>
#include <string>
#include <memory>

namespace chai
{
    class CORE_EXPORT IAsset
    {
    public:
        virtual ~IAsset() = default;
        virtual bool isValid() const = 0;
        virtual const std::string& getAssetId() const = 0;

    protected:
        bool m_valid{false};
        std::string m_assetId;
    };

    class CORE_EXPORT IAssetLoader
    {
    public:
        virtual ~IAssetLoader() = default;
        virtual bool canLoad(const std::string& extension) const = 0;
        virtual std::unique_ptr<IAsset> load(const std::string& path) = 0;
    };
}