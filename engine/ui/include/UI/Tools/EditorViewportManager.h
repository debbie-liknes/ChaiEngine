#pragma once
#include <Rendering/IRenderer.h>
#include <UI/Editor/PanelRegistry.h>
#include <string>

namespace chai::ui
{
	class EditorViewportManager
	{
    public:
        EditorViewportManager(gfx::IRenderer& renderer, PanelRegistry& panelRegistry);

        std::string addPane(gfx::IRenderer & renderer, const std::string& name, uint32_t cameraViewIndex);
	private:

		PanelRegistry& panelRegistry_;
	};
}