/**
 * @file PipelineBuilder.h
 * @brief Graphics pipeline creation helper
 */
#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace chai::gfx
{
    /**
     * @brief No VkRenderPass, with the intent to do dynamic rendering.
     * @todo This is not complete
     */
    class PipelineBuilder
    {
    public:
        PipelineBuilder& setShaders(VkShaderModule vert, VkShaderModule frag);
        PipelineBuilder& setTopology(VkPrimitiveTopology topology);
        PipelineBuilder& setPolygonMode(VkPolygonMode mode);
        PipelineBuilder& setCullMode(VkCullModeFlags cull, VkFrontFace front);
        PipelineBuilder& setColorFormat(VkFormat format);
        PipelineBuilder& setVertexInput(std::vector<VkVertexInputAttributeDescription> attr,
                                        VkVertexInputBindingDescription bind);

        // Depth
        PipelineBuilder& setDepthFormat(VkFormat depthFormat);
        PipelineBuilder& setDepthOp(VkCompareOp compareOp);
        PipelineBuilder& disableDepthTest();
        PipelineBuilder& enableDepthTest();
        PipelineBuilder& disableDepthWrite();
        PipelineBuilder& enableDepthWrite();
        PipelineBuilder& enableDepthBias();

        // blending
        PipelineBuilder& disableBlending();
        PipelineBuilder& enableBlending();

        PipelineBuilder& setSampleCount(VkSampleCountFlagBits);

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
        bool hasColor_ = false;
        std::vector<VkVertexInputAttributeDescription> attrs_{};
        VkVertexInputBindingDescription bind_{};
        bool blending_ = false;

        VkFormat depthFormat_ = VK_FORMAT_UNDEFINED;
        bool depthTest_ = false;
        bool depthWrite_ = false;
        bool depthBias_ = false;
        VkCompareOp depthCompareOp_ = VK_COMPARE_OP_LESS;

        bool hasVertexInput_ = false;
        VkSampleCountFlagBits sampleCount_ = VK_SAMPLE_COUNT_1_BIT;
    };
} // namespace chai::gfx