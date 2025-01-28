#pragma once
#include <Core/Containers.h>
#include <Core/MemoryTypes.h>

namespace CGraphics
{
	class Viewer;
	class Viewport;
	class Window;

	using SharedViewer = Core::CSharedPtr<Viewer>;
	using SharedViewport = Core::CSharedPtr<Viewport>;
}