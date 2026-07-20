#include "PipelineBuilder.h"

#include "../utils/VkCheck.h"
#include "../utils/VkUtils.h"

namespace chai::gfx
{
    PipelineBuilder& PipelineBuilder::setShaders(VkShaderModule vert, VkShaderModule frag)
    {
        vert_ = vert;
        frag_ = frag;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setTopology(VkPrimitiveTopology t)
    {
        topology_ = t;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setPolygonMode(VkPolygonMode m)
    {
        polygonMode_ = m;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setCullMode(VkCullModeFlags c, VkFrontFace f)
    {
        cullMode_ = c;
        frontFace_ = f;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setColorFormat(VkFormat f)
    {
        colorFormat_ = f;
        hasColor_ = true;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::disableDepthTest()
    {
        depthTest_ = false;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::enableDepthTest()
    {
        depthTest_ = true;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setDepthFormat(VkFormat depthFormat)
    {
        depthFormat_ = depthFormat;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::setDepthOp(VkCompareOp compareOp)
    {
        depthCompareOp_ = compareOp;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::disableDepthWrite()
    {
        depthWrite_ = false;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::enableDepthWrite()
    {
        depthWrite_ = true;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::enableDepthBias()
    {
        depthBias_ = true;
        return *this;
    }


    PipelineBuilder& PipelineBuilder::enableBlending()
    {
        blending_ = true;
        return *this;
    }

    PipelineBuilder& PipelineBuilder::disableBlending()
    {
        blending_ = false;
        return *this;
    }

    PipelineBuilder&
    PipelineBuilder::setVertexInput(std::vector<VkVertexInputAttributeDescription> attr,
                                    VkVertexInputBindingDescription bind)
    {
        attrs_ = std::move(attr);
        bind_ = bind;
        hasVertexInput_ = true; 
        return *this;
    }

    VkPipeline PipelineBuilder::build(VkDevice device, VkPipelineLayout layout)
    {
        // TODO: support adding multiple shader stages
        VkPipelineShaderStageCreateInfo stages[2]{};
        stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        stages[0].module = vert_;
        stages[0].pName = "main";
        stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        stages[1].module = frag_;
        stages[1].pName = "main";

        VkPipelineVertexInputStateCreateInfo vertexInput{
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
        if (hasVertexInput_) {
            vertexInput.vertexBindingDescriptionCount = 1;
            vertexInput.pVertexBindingDescriptions = &bind_;
            vertexInput.vertexAttributeDescriptionCount = uint32_t(attrs_.size());
            vertexInput.pVertexAttributeDescriptions = attrs_.data();
        }

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
        inputAssembly.topology = topology_;

        VkPipelineViewportStateCreateInfo viewport{
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
        viewport.viewportCount = 1;
        viewport.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo raster{
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
        raster.polygonMode = polygonMode_;
        raster.cullMode = cullMode_;
        raster.frontFace = frontFace_;
        raster.lineWidth = 1.0f;
        raster.depthBiasEnable = depthBias_ ? VK_TRUE : VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisample{
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
        multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // no MSAA
        multisample.minSampleShading = 1.0f;

        VkPipelineDepthStencilStateCreateInfo depthStencil{
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
        depthStencil.depthTestEnable = depthTest_ ? VK_TRUE : VK_FALSE;
        depthStencil.depthWriteEnable = depthWrite_ ? VK_TRUE : VK_FALSE;
        depthStencil.depthCompareOp = depthCompareOp_;
        depthStencil.maxDepthBounds = 1.0f;

        VkPipelineColorBlendAttachmentState blendAttachment{};
        blendAttachment.blendEnable = blending_ ? VK_TRUE : VK_FALSE;
        blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                         VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlend{
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
        colorBlend.attachmentCount = 1;
        colorBlend.pAttachments = &blendAttachment;

        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_DEPTH_BIAS};
        VkPipelineDynamicStateCreateInfo dynamic{
            VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
        dynamic.dynamicStateCount = 3;
        dynamic.pDynamicStates = dynamicStates;

        // Dynamic rendering hookup
        VkPipelineRenderingCreateInfo renderingInfo{
            VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
        if (hasColor_) {
            renderingInfo.colorAttachmentCount = 1;
            renderingInfo.pColorAttachmentFormats = &colorFormat_;
        }
        renderingInfo.depthAttachmentFormat = depthFormat_;

        VkGraphicsPipelineCreateInfo info{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
        info.pNext = &renderingInfo;
        info.stageCount = 2;
        info.pStages = stages;
        info.pVertexInputState = &vertexInput;
        info.pInputAssemblyState = &inputAssembly;
        info.pViewportState = &viewport;
        info.pRasterizationState = &raster;
        info.pMultisampleState = &multisample;
        info.pDepthStencilState = &depthStencil;
        info.pColorBlendState = &colorBlend;
        info.pDynamicState = &dynamic;
        info.layout = layout;
        info.renderPass = VK_NULL_HANDLE; // dynamic rendering = no render pass

        VkPipeline pipeline = VK_NULL_HANDLE;
        VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline));
        return pipeline;
    }
} // namespace chai::gfx