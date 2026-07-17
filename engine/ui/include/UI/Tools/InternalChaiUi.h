/**
 * @file InternalChaiUI.h
 * @brief UI for internals and tooling. NOT for production use.
 */
#pragma once
#include <string>
#include <vector>
#include <ChaiMath.h>

namespace chai::ui
{
    // this is imgui render backend interface. We will for now provide the implementation in the vulkan backend
    class IInternalChaiUi
    {
    public:
        virtual ~IInternalChaiUi() = default;
        virtual bool initializeUI() = 0;
        virtual void shutdownUI() = 0;
    };
}