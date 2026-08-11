#include "PipelineCache.h"

#include "../resources/VulkanVertex.h"
#include "../utils/VkCheck.h"
#include "PipelineHelpers.h"

namespace chai::gfx
{
    PipelineCache::PipelineCache(VulkanContext& ctx, VkFormat colorFormat, VkFormat depthFormat)
        : ctx_(ctx), colorFormat_(colorFormat), depthFormat_(depthFormat)
    {
        {
            VkPushConstantRange pcRange{};
            pcRange.offset = 0;
            pcRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            pcRange.size = sizeof(PushConstants);

            // set 0 = camera, set 1 = material, set 2 = light, set 3 = env
            VkDescriptorSetLayout setLayouts[] = {ctx_.cameraSetLayout(),
                                                  ctx_.materialSetLayout(),
                                                  ctx_.lightSetLayout(),
                                                  ctx_.environmentSetLayout()};
            VkPipelineLayoutCreateInfo layoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
            layoutInfo.pushConstantRangeCount = 1;
            layoutInfo.pPushConstantRanges = &pcRange;
            layoutInfo.setLayoutCount = 4;
            layoutInfo.pSetLayouts = setLayouts;

            VK_CHECK(vkCreatePipelineLayout(ctx_.device(), &layoutInfo, nullptr, &pipelineLayout_));
        }

        attrs_ = vertexAttributes();
        binding_ = vertexBinding();
    }

    void PipelineCache::destroyAll()
    {
        for (auto& [key, pipe] : pipelines_) {
            vkDestroyPipeline(ctx_.device(), pipe, nullptr);
        }

        vkDestroyPipelineLayout(ctx_.device(), pipelineLayout_, nullptr);
    }

    VkPipeline PipelineCache::getOrCreate(const PipelineKey& key)
    {
        auto it = pipelines_.find(key);
        if (it != pipelines_.end()) {
            return it->second;
        }

        // Create a new pipeline
        VkPipeline pipeline = createPipeline(key);
        pipelines_[key] = pipeline;
        return pipeline;
    }

    VkPipeline PipelineCache::createPipeline(const PipelineKey& key)
    {
        VkPipeline pipeline = loadPipelineByName(
            ctx_, key.vertShader, key.fragShader, pipelineLayout_, [&](PipelineBuilder& b) {
                b.setVertexInput({attrs_.begin(), attrs_.end()}, binding_)
                    .setColorFormat(colorFormat_)
                    .setDepthFormat(depthFormat_)
                    .enableDepthTest()
                    .setPolygonMode(key.polygon)
                    .setSampleCount(key.sampleCount)
                    .setCullMode(key.polygon != VK_POLYGON_MODE_FILL ? VK_CULL_MODE_NONE : VK_CULL_MODE_BACK_BIT,
                                 VK_FRONT_FACE_COUNTER_CLOCKWISE);

                if (key.alpha == AlphaMode::Blend) {
                    b.enableBlending();
                    b.disableDepthWrite();
                } else {
                    b.disableBlending();
                    b.enableDepthWrite();
                }
            });
        return pipeline;
    }
} // namespace chai::gfx