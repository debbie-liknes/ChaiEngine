#pragma once
#include <Core/MemoryTypes.h>

namespace CGraphics
{
	class Viewer;
	class Viewport;

	using SharedViewer = Core::CSharedPtr<Viewer>;
	using SharedViewport = Core::CSharedPtr<Viewport>;
}