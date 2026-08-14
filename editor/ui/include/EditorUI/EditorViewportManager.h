#pragma once
#include <Rendering/IRenderer.h>
#include <Rendering/Viewport.h>
#include <EditorUI/PanelRegistry.h>
#include <string>
#include <Rendering/Viewport.h>

namespace chai::ui
{
	class EditorViewportManager
	{
    public:
        EditorViewportManager(gfx::IViewportRegistry& registry, PanelRegistry& panelRegistry);

        std::string addViewport(const std::string& name, uint32_t cameraViewId);
	private:

		PanelRegistry& panelRegistry_;
        gfx::IViewportRegistry& registry_;
	};
}