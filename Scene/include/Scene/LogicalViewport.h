#pragma once
#include <Core/Rect.h>
#include <Scene/Camera.h>

namespace chai
{
	class LogicalViewport
	{
		Rect screenRect; // The rectangle representing the viewport
		brew::Camera* camera;
	};
}