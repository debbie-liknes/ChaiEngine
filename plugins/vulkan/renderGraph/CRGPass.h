/**
 * @file CRGPass.h
 */
#pragma once
#include "CRGDescriptors.h"
#include "CRGResources.h"

#include <string>

namespace chai::gfx
{
    /**
     * @brief Typed erased base class for a Pass. This is owned by the render graph
     */
    class CRGPassBase
    {
    public:
        virtual ~CRGPassBase() = default;
        std::string name;
        std::vector<CRGTextureAccess> accesses;

        virtual void execute(CRGResources& res, VkCommandBuffer cmd) = 0;
    };

    /**
     * @brief Typed class for a Pass
     */
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