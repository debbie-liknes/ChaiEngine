#pragma once
#include <ChaiMath.h>

namespace chai::gfx
{
    using namespace chai::math;
	struct Vertex
	{
        Vec3 position;
        Vec3 normal;
        Vec2 uv;
        Vec4 tangent; // xyz = tangent; w = bit handedness

        friend bool operator==(const Vertex&, const Vertex&) = default;
	};
}