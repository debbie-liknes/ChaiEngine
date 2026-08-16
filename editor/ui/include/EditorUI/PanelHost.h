#pragma once
#include <EditorUI/DockspaceService.h>
#include <EditorUI/PanelRegistry.h>
#include <EditorUI/ActionManager.h>

namespace chai::ui
{
	class PanelHost
	{
    public:
        void draw(PanelRegistry& registry, DockspaceService& dockspace, ActionManager& menus);
	};
}