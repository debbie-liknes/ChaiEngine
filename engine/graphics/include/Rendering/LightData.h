#pragma once
#include <ChaiMath.h>
#include <Common/GraphicsMacros.h>

namespace chai::gfx
{
	struct alignas(16) LightData
	{
        math::Vec4 direction;
        math::Vec4 color;
	};
    ENFORCE_STD140_ALIGNMENT(LightData);
}