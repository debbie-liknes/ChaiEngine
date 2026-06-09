/**
 * @file PipelineBuilder.h
 * @brief Graphics pipeline creation helper
 */
#pragma once
#include <vulkan/vulkan.h>

namespace chai::gfx
{
    /**
     * @brief No VkRenderPass, with the intent to do dynamic rendering.
     */
    class PipelineBuilder
    {
    public:
        PipelineBuilder& setShaders(VkShaderModule vert, VkShaderModule frag);
        PipelineBuilder& setTopology(VkPrimitiveTopology topology);
        PipelineBuilder& setPolygonMode(VkPolygonMode mode);
        PipelineBuilder& setCullMode(VkCullModeFlags cull, VkFrontFace front);
        PipelineBuilder& setColorFormat(VkFormat format);
        PipelineBuilder& disableDepthTest();
        PipelineBuilder& disableBlending();

        /**
         * @brief Assembles the create struct and returns the pipeline.
         */
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