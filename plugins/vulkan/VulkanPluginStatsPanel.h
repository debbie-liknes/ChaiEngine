#pragma once
#include <UI/Editor/InternalChaiUI.h>
#include <UI/Editor/TreeNode.h>
#include "renderer/VulkanRenderer.h"

// I know this plugin has ImGui right now because it has the vulkan impl, but DO NOT
// include ImGui headers here. It should always use chai::ui::Text() and other chai::ui functions
namespace chai::ui
{
	void drawVulkanStatsPanel(const gfx::VulkanStats& stats)
	{
        Text("Total GPU Frame Time: " + std::to_string(stats.gpuTimeMs) + " ms");

        {
            TreeNode main("Main Pass", "mainPassNode");
            if (main) {
                Text("Draw Calls: " + std::to_string(stats.mainPass.drawCalls));
                Text("GPU Time: " + std::to_string(stats.mainPass.gpuTimeMs) + " ms");
            }
        }

        {
            TreeNode shadow("Shadow Pass", "shadowPassNode");
            if (shadow) {
                Text("Draw Calls: " + std::to_string(stats.shadowPass.drawCalls));
                Text("GPU Time: " + std::to_string(stats.shadowPass.gpuTimeMs) + " ms");
            }
        }
	}

    void drawRenderDebugTools(gfx::VulkanRenderer& renderer) 
    {
        if (Button("Hot Reload")) {
            renderer.recompileShaders();
        }
    }
} // namespace chai::ui