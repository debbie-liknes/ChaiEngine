#pragma once
#include <ChaiEngine/RenderFrame.h>
#include <Scene/Scene.h>
#include <Window/Window.h>
#include <ChaiEngine/RenderView.h>

namespace chai::cup
{
	//intended to bind together cameras/views/viewports to a render frame
	class RenderFrameBuilder
	{
	public:
		static brew::RenderFrame build(const Scene& scene, Viewport* vp);
	};
}