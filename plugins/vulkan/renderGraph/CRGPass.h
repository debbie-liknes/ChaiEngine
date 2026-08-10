#pragma once
#include <string>
#include "CRGDescriptors.h"
#include "CRGResources.h"

namespace chai::gfx
{
    class CRGPassBase
    {
    public:
        virtual ~CRGPassBase() = default;
        std::string name;
        std::vector<CRGTextureAccess> accesses;

        virtual void execute(CRGResources& res, VkCommandBuffer cmd) = 0;
    };

    template <typename PassData>
    class CRGPass : public CRGPassBase
    {
    public:
        PassData data;
        std::function<void(CRGResources&, VkCommandBuffer)> executeFn;

        void execute(CRGResources& res, VkCommandBuffer cmd) override
        {
            if (executeFn)
                executeFn(res, cmd);
        }
    };
} // namespace chai::gfx