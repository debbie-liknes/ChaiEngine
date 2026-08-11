#pragma once
#include "../VulkanCommon.h"
#include "../resources/RenderTargetView.h"

namespace chai::gfx
{
    struct Viewport {
        std::string id;
        ViewportTarget targets[kFramesInFlight];
        VkBuffer cameraBuffer[kFramesInFlight]{};
        VmaAllocation cameraAlloc[kFramesInFlight]{};
        void* cameraMapped[kFramesInFlight]{};
        VkDescriptorSet cameraSet[kFramesInFlight]{};
        bool everRendered[kFramesInFlight] = {false, false};
        VkExtent2D pendingExtent{};
        bool needsResize = false;
        bool hovered = false;
        uint32_t cameraViewId = 0;
        bool wireframe = false;
        ViewportShadingMode shadingMode = ViewportShadingMode::Lit;
    };

    struct ViewportSlot {
        Viewport viewport;
        uint32_t generation = 0;
        bool alive = false;
    };
}