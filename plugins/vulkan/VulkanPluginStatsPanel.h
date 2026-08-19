#pragma once
#include <UI/Core/Buttons.h>
#include <UI/Core/TreeNode.h>
#include "renderer/VulkanRenderer.h"
#include <UI/Core/Containers/Panel.h>
#include <memory>

// I know this plugin has ImGui right now because it has the vulkan impl, but DO NOT
// include ImGui headers here. It should always use chai::ui::Text() and other chai::ui functions
namespace chai::ui
{
    std::unique_ptr<ui::Panel> createVulkanStatsPanel(const gfx::VulkanStats& stats)
	{
        auto panel = std::make_unique<ui::Panel>("Vulkan Stats");
        auto& button = panel->add<ui::Button>("Clear");
        //button.onClick = [&renderer]() { renderer.recompileShaders(); };

        panel->visible = false;

        //Text("Total GPU Frame Time: " + std::to_string(stats.gpuTimeMs) + " ms");

        //for (auto& pass : stats.allPasses) {
        //    TreeNode passNode(pass.name.c_str(), pass.name.c_str());
        //    if (passNode) {
        //        Indent();
        //        Text("Draw Calls: " + std::to_string(pass.drawCalls));
        //        Text("GPU Time: " + std::to_string(pass.gpuTimeMs) + " ms");
        //        Unindent();
        //    }
        //}

        return std::move(panel);
	}

    std::unique_ptr<ui::Panel> createRenderDebugPanel(gfx::VulkanRenderer& renderer) 
    {
        auto panel = std::make_unique<ui::Panel>("Render Debug");
        auto& button = panel->add<ui::Button>("Clear");
        button.onClick = [&renderer]() { renderer.recompileShaders(); };

        panel->visible = false;
        return std::move(panel);
    }
} // namespace chai::ui