/**
 * @file LightData.h
 */
#pragma once
#include <ChaiMath.h>
#include <Common/GraphicsMacros.h>

namespace chai::gfx
{
    /**
     * @brief GPU data for lights. Only supports a directional light
     */
	struct alignas(16) LightData
	{
        math::Vec4 direction;
        math::Vec4 color;
        math::Mat4 view;
        math::Mat4 proj;
	};
    ENFORCE_STD140_ALIGNMENT(LightData);
}