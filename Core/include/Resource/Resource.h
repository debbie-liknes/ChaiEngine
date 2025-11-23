#pragma once
#include <CoreExport.h>
#include <Asset/AssetHandle.h>

namespace chai
{
    struct CORE_EXPORT Resource
    {
        explicit Resource(AssetHandle sourceAsset)
        {
        }

        Resource() = default;
        virtual ~Resource() = default;
    };
}