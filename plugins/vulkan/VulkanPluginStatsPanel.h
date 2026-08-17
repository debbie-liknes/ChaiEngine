#pragma once
#include <UI/Core/InternalChaiUI.h>
#include <UI/Core/TreeNode.h>
#include "renderer/VulkanRenderer.h"

// I know this plugin has ImGui right now because it has the vulkan impl, but DO NOT
// include ImGui headers here. It should always use chai::ui::Text() and other chai::ui functions
namespace chai::ui
{
	void drawVulkanStatsPanel(const gfx::VulkanStats& stats)
	{
        Text("Total GPU Frame Time: " + std::to_string(stats.gpuTimeMs) + " ms");

        for (auto& pass : stats.allPasses) {
            TreeNode passNode(pass.name.c_str(), pass.name.c_str());
            if (passNode) {
                Indent();
                Text("Draw Calls: " + std::to_string(pass.drawCalls));
                Text("GPU Time: " + std::to_string(pass.gpuTimeMs) + " ms");
                Unindent();
            }
        }
	}

    void drawRenderDebugTools(gfx::VulkanRenderer& renderer) 
    {
        if (Button("Reload Shaders")) {
            renderer.recompileShaders();
        }
    }
} // namespace chai::ui