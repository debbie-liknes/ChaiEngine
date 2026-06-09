#pragma once
#include <vulkan/vulkan.h>

namespace chai::gfx
{
    // Graphics pipeline creation is one giant nest of CreateInfo structs. This
    // collects the decisions as readable steps, then assembles them in build().
    //
    // Dynamic-rendering shaped: there is NO VkRenderPass. build() chains a
    // VkPipelineRenderingCreateInfo (carrying the color format) into pNext, so
    // the pipeline is compatible with any target of that format — swapchain or
    // offscreen alike. That's what keeps pipelines target-agnostic.
    //
    // For the triangle: hardcoded verts (empty vertex input), dynamic viewport/
    // scissor, no depth, no blend, no culling.
    class PipelineBuilder
    {
    public:
        PipelineBuilder& setShaders(VkShaderModule vert, VkShaderModule frag);
        PipelineBuilder& setTopology(VkPrimitiveTopology topology);
        PipelineBuilder& setPolygonMode(VkPolygonMode mode);
        PipelineBuilder& setCullMode(VkCullModeFlags cull, VkFrontFace front);
        PipelineBuilder& setColorFormat(VkFormat format); // for dynamic rendering
        PipelineBuilder& disableDepthTest();
        PipelineBuilder& disableBlending();

        // Assembles the create struct and returns the pipeline. The layout and
        // shader modules are borrowed — caller owns and destroys them.
        VkPipeline build(VkDevice device, VkPipelineLayout layout);

    private:
        VkShaderModule vert_ = VK_NULL_HANDLE;
        VkShaderModule frag_ = VK_NULL_HANDLE;
        VkPrimitiveTopology topology_ = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkPolygonMode polygonMode_ = VK_POLYGON_MODE_FILL;
        VkCullModeFlags cullMode_ = VK_CULL_MODE_NONE;
        VkFrontFace frontFace_ = VK_FRONT_FACE_CLOCKWISE;
        VkFormat colorFormat_ = VK_FORMAT_UNDEFINED;
        bool depthTest_ = false;
        bool blending_ = false;
    };
} // namespace chai