#pragma once
#include <ChaiMath.h>

namespace chai
{
    struct AABB
    {
        Vec3 min;
        Vec3 max;

        Vec3 GetPositiveVertex(const Vec3& normal) const;
        Vec3 GetNegativeVertex(const Vec3& normal) const;
    };
}