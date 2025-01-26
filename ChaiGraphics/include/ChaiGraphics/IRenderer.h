#pragma once
#include <ChaiGraphicsExport.h>

namespace CGraphics
{
	class CHAIGRAPHICS_EXPORT IRenderer
	{
	public:
		IRenderer();
		~IRenderer() = default;

		void render() {}

		//IRenderer(const IRenderer& r) = delete;
		//IRenderer& operator=(const IRenderer& other) = delete;

		virtual void setProcAddress(void* address) {}
	};
}
