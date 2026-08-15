#include "ChaiRenderGraph.h"

#include "CRGResources.h"

#include <Graph/Algorithms.h>

#include <Log.h>
#include <queue>

namespace chai::gfx
{
    graph::Graph<CRGPassBase*> buildPassGraph(std::vector<std::unique_ptr<CRGPassBase>>& passes);

    ChaiRenderGraph::ChaiRenderGraph(VulkanContext& ctx) : ctx_(ctx) {}

    ChaiRenderGraph::~ChaiRenderGraph()
    {
        for (auto& tex : textures_) {
            if (!tex.isImported)
                tex.target.destroy(ctx_);
        }
    }

    // TODO: actually handle the generational part

    CRGTextureHandle ChaiRenderGraph::importTexture(const std::string& name,
                                                    RenderTarget& view,
                                                    ImageState state,
                                                    TextureType type)
    {
        for (uint32_t i = 0; i < textures_.size(); ++i) {
            if (textures_[i].name == name && textures_[i].isImported) {
                // re-point at the real resource
                textures_[i].importedTarget = &view;

                // refill the state, dont trust last mip state, never trust a last image state (dont
                // trust me)
                std::fill(textures_[i].mipStates.begin(), textures_[i].mipStates.end(), state);

                return CRGTextureHandle{i, textures_[i].generation};
            }
        }

        // not found
        CRGTexture tex;
        tex.name = name;
        tex.isImported = true;
        tex.importedTarget = &view;
        tex.mipStates.assign(1, state); // single mip? who can say
        tex.desc.type = type;
        textures_.push_back(std::move(tex));
        return CRGTextureHandle{uint32_t(textures_.size() - 1), textures_.back().generation};
    }

    VkImage ChaiRenderGraph::resolvedImage(CRGTextureHandle handle) const
    {
        const CRGTexture& tex = textures_[handle.index];
        return tex.isImported ? tex.importedTarget->image : tex.target.image;
    }

    VkExtent2D ChaiRenderGraph::resolvedExtent(CRGTextureHandle handle, uint32_t mip) const
    {
        const CRGTexture& tex = textures_[handle.index];
        VkExtent2D base = tex.isImported ? tex.importedTarget->extent : tex.target.extent;
        return {std::max(1u, base.width >> mip), std::max(1u, base.height >> mip)};
    }

    CRGTexture ChaiRenderGraph::buildTexture(const std::string& name, const CRGTextureDesc& desc)
    {
        CRGTexture texture{};
        texture.isImported = false;
        if (desc.type == TextureType::Color2D) {
            texture.target = createColor2D(
                ctx_, desc.width, desc.height, desc.format, desc.mipLevels, desc.sampleCount);

        } else if (desc.type == TextureType::Depth) {
            texture.target =
                createDepth2D(ctx_, desc.width, desc.height, desc.format, false, desc.sampleCount);
        }
        texture.mipStates.resize(desc.mipLevels, ImageState::Undefined);
        texture.desc = desc;
        texture.name = name;
        return texture;
    }

    CRGTextureHandle ChaiRenderGraph::createTexture(const std::string& name,
                                                    const CRGTextureDesc& desc)
    {
        // check the pool for an existing match
        for (uint32_t i = 0; i < textures_.size(); ++i) {
            if (textures_[i].name == name && !textures_[i].isImported) {
                if (textures_[i].desc.width == desc.width &&
                    textures_[i].desc.height == desc.height &&
                    textures_[i].desc.format == desc.format &&
                    textures_[i].desc.mipLevels == desc.mipLevels) {
                    return CRGTextureHandle{i, textures_[i].generation};
                } else {
                    // desc changed
                    vkDeviceWaitIdle(ctx_.device()); // TODO: replace this with deferred deletion
                    textures_[i].target.destroy(ctx_);
                    textures_[i].generation++;
                    textures_[i] = buildTexture(name, desc); // recreate at the SAME index
                    textures_[i].generation += 1;
                    return CRGTextureHandle{i, textures_[i].generation};
                }
            }
        }

        // no match
        CRGTexture tex = buildTexture(name, desc);
        textures_.push_back(std::move(tex));
        return CRGTextureHandle{uint32_t(textures_.size() - 1), textures_.back().generation};
    }

    bool ChaiRenderGraph::compile()
    {
        // sort and compute
        if (auto passGraph = buildPassGraph(passes_);
            !graph::topologicalSort(passGraph, executionOrder_)) {
            CHAI_LOG_CRITICAL("RenderGraph: cycle detected. Some pass depends on itself indirectly.");
            return false;
        }
        computeBarriers(executionOrder_, textures_);

        return true;
    }

    void ChaiRenderGraph::clear()
    {
        passes_.clear();
        executionOrder_.clear();
        barrierPlan_.clear();
    }

    void ChaiRenderGraph::execute(VkCommandBuffer cmd)
    {
        // walk through the passes in order
        for (uint32_t passIdx : executionOrder_) {
            // transition all images
            for (auto& barrier : barrierPlan_[passIdx])
                transitionImage(cmd, barrier.image, barrier.from, barrier.to, barrier.mip);

            // execute the callers logic
            CRGResources res(*this);
            passes_[passIdx]->execute(res, cmd);
        }
    }

    void ChaiRenderGraph::computeBarriers(const std::vector<uint32_t>& order,
                                          std::vector<CRGTexture>& textures)
    {
        barrierPlan_.clear();

        for (uint32_t passIdx : order) {
            for (auto& access : passes_[passIdx]->accesses) {
                CRGTexture& tex = textures[access.handle.index];
                ImageState needed = ImageState::Undefined;
                if (tex.desc.type == TextureType::Color2D) {
                    needed = (access.access == CRGAccess::Read) ? ImageState::ShaderRead
                                                                : ImageState::ColorAttachment;
                } else if (tex.desc.type == TextureType::Depth) {
                    needed = (access.access == CRGAccess::Read) ? ImageState::DepthShaderRead
                                                                : ImageState::DepthAttachment;
                }

                ImageState current = tex.mipStates[access.mip];

                if (current != needed) {
                    barrierPlan_[passIdx].push_back({tex.image(), current, needed, access.mip});
                    tex.mipStates[access.mip] = needed;
                }
                // if current == needed already, no barrier
            }
        }
    }

    graph::Graph<CRGPassBase*> buildPassGraph(std::vector<std::unique_ptr<CRGPassBase>>& passes)
    {
        graph::Graph<CRGPassBase*> passGraph;

        // combines handle index & mip into one lookup key
        auto makeKey = [](uint32_t handleIndex, uint32_t mip) {
            return (uint64_t(handleIndex) << 32) | uint64_t(mip);
        };

        std::unordered_map<uint64_t, uint32_t>
            lastWriter; // maps slot to pass index that most recently wrote it

        for (uint32_t i = 0; i < passes.size(); i++) {
            const auto& pass = passes[i];

            passGraph.nodes.push_back(pass.get());

            // resolve reads first
            for (const auto& access : pass->accesses) {
                if (access.access == CRGAccess::Read) {
                    uint64_t k = makeKey(access.handle.index, access.mip);
                    if (auto it = lastWriter.find(k); it != lastWriter.end()) {
                        passGraph.edges.add(it->second, i);
                    }
                }
            }

            // register this pass's writes, so later passes see them
            for (const auto& access : pass->accesses) {
                if (access.access == CRGAccess::Write) {
                    uint64_t k = makeKey(access.handle.index, access.mip);
                    lastWriter[k] = i;
                }
            }
        }

        return passGraph;
    }
} // namespace chai::gfx