#include <EditorUI/DockspaceService.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace chai::ui
{
    ImGuiDir toImGuiDir(DockSplit::Side side)
    {
        if (side == DockSplit::Side::Top)
            return ImGuiDir_Up;
        if (side == DockSplit::Side::Bottom)
            return ImGuiDir_Down;
        if (side == DockSplit::Side::Right)
            return ImGuiDir_Right;
        if (side == DockSplit::Side::Left)
            return ImGuiDir_Left;

        return ImGuiDir_None;
    }

    DockId DockspaceService::begin()
    {
        ImGuiID dockspaceId = ImGui::GetID("MainDockspace");
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        if (!defaultLayoutBuilt_) {
            defaultLayoutBuilt_ = true;
            if (ImGui::DockBuilderGetNode(dockspaceId) == nullptr) {
                ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_PassthruCentralNode);
                ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->WorkSize);

                ImGuiID remaining = dockspaceId;
                for (auto& split : splits_) {
                    ImGuiID sideId;
                    ImGuiID oppositeId;
                    ImGui::DockBuilderSplitNode(
                        remaining, toImGuiDir(split.side), split.ratio, &sideId, &oppositeId);
                    ImGui::DockBuilderDockWindow(split.windowId.c_str(), sideId);
                    remaining = oppositeId;
                }
                ImGui::DockBuilderDockWindow(centerWindowId_.c_str(), remaining);

                ImGui::DockBuilderFinish(dockspaceId);
            }
        }

        return static_cast<DockId>(ImGui::DockSpaceOverViewport(dockspaceId, viewport, ImGuiDockNodeFlags_PassthruCentralNode));
    }

    void DockspaceService::setDefaultLayout(std::vector<DockSplit> splits, std::string centerWindowId)
    {
        splits_ = std::move(splits);
        centerWindowId_ = centerWindowId;
    }
}