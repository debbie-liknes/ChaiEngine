#include "ChaiRenderGraph.h"
#include "CRGResources.h"

#include <Log.h>
#include <queue>

namespace chai::gfx
{
    ChaiRenderGraph::ChaiRenderGraph(VulkanContext& ctx) : ctx_(ctx) {}

    // TODO: actually handle the generational part

    CRGTextureHandle ChaiRenderGraph::importTexture(const std::string& name,
                                                    RenderTargetView& view,
                                                    ImageState state)
    {
        CRGTexture texture{};
        texture.isImported = true;
        texture.importedTarget = &view;
        texture.mipStates.resize(1, state);
        textures_.push_back(std::move(texture));
        return CRGTextureHandle{static_cast<uint32_t>(textures_.size() - 1), 0};
    }

    CRGTextureHandle ChaiRenderGraph::createTexture(const std::string& name,
                                                    const CRGTextureDesc& desc)
    {
        CRGTexture texture{};
        texture.isImported = false;
        texture.target = createColor2D(ctx_, desc.width, desc.height, desc.format);
        texture.mipStates.resize(desc.mipLevels, ImageState::Undefined);
        textures_.push_back(std::move(texture));
        return CRGTextureHandle{static_cast<uint32_t>(textures_.size() - 1), 0};
    }

    void ChaiRenderGraph::compile()
    {
        executionOrder_ = topologicalSort(passes_);
        computeBarriers(executionOrder_, textures_);
    }

    void ChaiRenderGraph::execute(VkCommandBuffer cmd)
    {
        for (uint32_t passIdx : executionOrder_) {
            for (auto& barrier : barrierPlan_[passIdx])
                transitionImage(cmd, barrier.image, barrier.from, barrier.to, barrier.mip);

            CRGResources res(*this);
            passes_[passIdx]->execute(res, cmd);
        }
    }

    std::vector<uint32_t>
    ChaiRenderGraph::topologicalSort(std::vector<std::unique_ptr<CRGPassBase>>& passes)
    {
        std::unordered_map<uint32_t, std::vector<uint32_t>> adjList;
        std::vector<uint32_t> inDegree;
        buildAdjacencyList(passes, adjList, inDegree);

        std::queue<uint32_t> ready;
        for (uint32_t i = 0; i < passes.size(); ++i)
            if (inDegree[i] == 0)
                ready.push(i);

        // kahns algo
        std::vector<uint32_t> order;
        while (!ready.empty()) {
            uint32_t p = ready.front();
            ready.pop();
            order.push_back(p);
            for (uint32_t dependent : adjList[p]) {
                if (--inDegree[dependent] == 0)
                    ready.push(dependent); // this pass last dependency just got resolved
            }
        }

        if (order.size() != passes.size())
            CHAI_LOG_ERROR("RenderGraph: cycle detected. Some pass depends on itself indirectly.");

        return order;
    }

    void ChaiRenderGraph::computeBarriers(const std::vector<uint32_t>& order,
                                          std::vector<CRGTexture>& textures)
    {
        barrierPlan_.clear();

        for (uint32_t passIdx : order) {
            for (auto& access : passes_[passIdx]->accesses) {
                CRGTexture& tex = textures[access.handle.index];
                ImageState needed = (access.access == CRGAccess::Read)
                                        ? ImageState::ShaderRead
                                        : ImageState::ColorAttachment;
                ImageState current = tex.mipStates[access.mip];

                if (current != needed) {
                    barrierPlan_[passIdx].push_back({tex.image(), current, needed, access.mip});
                    tex.mipStates[access.mip] = needed;
                }
                // if current == needed already, no barrier
            }
        }
    }

void ChaiRenderGraph::buildAdjacencyList(
        std::vector<std::unique_ptr<CRGPassBase>>& passes,
        std::unordered_map<uint32_t, std::vector<uint32_t>>& adjList,
        std::vector<uint32_t>& inDegree)
    {
        adjList.clear();
        inDegree.assign(passes.size(), 0);

        // key: combines handle index + mip into one lookup key
        auto makeKey = [](uint32_t handleIndex, uint32_t mip) {
            return (uint64_t(handleIndex) << 32) | uint64_t(mip);
        };

        std::unordered_map<uint64_t, uint32_t>
            lastWriter; // slot -> pass index that most recently wrote it

        for (uint32_t i = 0; i < passes.size(); i++) {
            auto& pass = passes[i];

            // resolve reads FIRST using writers seen so far (i.e. strictly earlier passes)
            for (auto& access : pass->accesses) {
                if (access.access != CRGAccess::Read)
                    continue;

                uint64_t k = makeKey(access.handle.index, access.mip);
                auto it = lastWriter.find(k);
                if (it != lastWriter.end()) {
                    adjList[it->second].push_back(i);
                    inDegree[i]++;
                }
                // if no writer seen yet, this read has no in-graph dependency
                // (e.g. reading an imported resource nothing in this graph writes)
            }

            // THEN register this pass's own writes, so later passes see them
            for (auto& access : pass->accesses) {
                if (access.access == CRGAccess::Write) {
                    uint64_t k = makeKey(access.handle.index, access.mip);
                    lastWriter[k] = i;
                }
            }
        }
    }
} // namespace chai::gfx