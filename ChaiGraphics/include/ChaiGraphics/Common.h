#pragma once
#include <memory>

namespace CGraphics
{
	class Viewer;
	class Viewport;

	using SharedViewer = std::shared_ptr<Viewer>;
	using SharedViewport = std::shared_ptr<Viewport>;
}