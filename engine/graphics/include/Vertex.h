/**
 * @file Vertex.h
 */
#pragma once
#include <ChaiMath.h>

namespace chai::gfx
{
    /**
     * @brief Holds attributes for every vertex
     */
	struct Vertex
	{
        math::Vec3 position;
        math::Vec3 normal;
        math::Vec2 uv;
        math::Vec4 tangent; // xyz = tangent, w = bit handedness

        friend bool operator==(const Vertex&, const Vertex&) = default;
	};
}