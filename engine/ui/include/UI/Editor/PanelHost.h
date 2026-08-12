#pragma once
#include <UI/Editor/DockspaceService.h>
#include <UI/Editor/PanelRegistry.h>
#include <UI/Editor/ActionManager.h>

namespace chai::ui
{
	class PanelHost
	{
    public:
        void draw(PanelRegistry& registry, DockspaceService& dockspace, ActionManager& menus);
	};
}