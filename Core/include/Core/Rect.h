#pragma once
#include <CoreExport.h>

namespace chai
{
	class CORE_EXPORT Rect
	{
	public:
		Rect() : x(0), y(0), width(0), height(0) {}
		Rect(int x, int y, int width, int height)
			: x(x), y(y), width(width), height(height) {}

		int x;      // X coordinate of the rectangle's top-left corner
		int y;      // Y coordinate of the rectangle's top-left corner
		int width;  // Width of the rectangle
		int height; // Height of the rectangle

		bool operator==(const Rect& other) const
		{
			return (x == other.x && y == other.y && width == other.width && height == other.height);
		}
	};
}