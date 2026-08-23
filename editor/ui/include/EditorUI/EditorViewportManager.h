#pragma once
#include <Rendering/IRenderer.h>
#include <Rendering/Viewport.h>
#include <EditorUI/PanelRegistry.h>
#include <string>
#include <Rendering/Viewport.h>
#include <Scene/Object.h>

namespace chai::ui
{
	class EditorViewportManager
	{
    public:
        EditorViewportManager(gfx::IViewportRegistry& registry, PanelRegistry& panelRegistry);

        std::string addViewport(const std::string& name, scene::ObjectId cameraViewId);
	private:

		PanelRegistry& panelRegistry_;
        gfx::IViewportRegistry& registry_;
	};
}
