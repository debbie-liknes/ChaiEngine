#pragma once
#include <UI/Editor/DockspaceService.h>
#include <UI/Editor/PanelRegistry.h>

namespace chai::ui
{
	class PanelHost
	{
    public:
        void draw(PanelRegistry& registry, DockspaceService& dockspace);
	};
}