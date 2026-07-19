#pragma once
#include <Rendering/IRenderer.h>
#include <UI/Editor/PanelRegistry.h>
#include <string>
#include <Rendering/Viewport.h>

namespace chai::ui
{
	class EditorViewportManager
	{
    public:
        EditorViewportManager(gfx::IRenderer& renderer, PanelRegistry& panelRegistry);

        std::string addViewport(const std::string& name, uint32_t cameraViewId);
	private:

		PanelRegistry& panelRegistry_;
        gfx::IRenderer& renderer_;
	};
}