#pragma once
#include <Window/Viewport.h>
#include <Meta/ChaiMacros.h>

namespace chai::brew
{
	class GLViewport : public Viewport
	{
	public:
		GLViewport();
		void bind() override;
	};

	CHAI_CLASS(chai::brew::GLViewport)
		CHAI_METHOD(bind)
	END_CHAI()
}