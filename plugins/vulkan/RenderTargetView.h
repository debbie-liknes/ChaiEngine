#pragma once
#include <vulkan/vulkan.h>

namespace chai::gfx
{
    // A plain description of "where to draw this frame." NOT an interface and
    // NOT owned here — it's a lightweight value handed to scene rendering.
    //
    // The whole point: renderScene(cmd, view) takes one of these and does not
    // know or care whether it came from the swapchain (present it) or an
    // offscreen target (sample it later, e.g. an editor panel). Same drawing
    // code, different producer. That decoupling is what enables multiple views
    // in one window down the line.
    //
    // Dynamic-rendering shaped: a target is just image views + formats, no
    // VkRenderPass / VkFramebuffer object zoo.
    struct RenderTargetView {
        VkExtent2D extent{};

        VkImage image = VK_NULL_HANDLE;         // for layout barriers
        VkImageView colorView = VK_NULL_HANDLE; // what we render into
        VkFormat colorFormat = VK_FORMAT_UNDEFINED;
        VkClearValue clearColor{}; // loadOp = CLEAR uses this

        // Depth comes later when there's geometry that needs it:
        // VkImageView depthView = VK_NULL_HANDLE;
        // VkFormat    depthFormat = VK_FORMAT_UNDEFINED;
    };
} // namespace chai