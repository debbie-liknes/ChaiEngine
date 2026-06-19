#pragma once

#include <ChaiMath.h>

namespace chai
{
    // Axis-aligned Bounding Box
    struct aabb
    {
        bool operator==(const aabb&) const;

        float getSurfaceArea() const;
        aabb expand(const aabb& box) const;

        // The center point in the AABB
        math::Vec3 center;
        float width;
        float length;
        float height;
    };

    struct BoxCollider
    {
        aabb box;

        // Local rotation is represented by centroid starting point
        // multiplied by quaternion for rotational axis + degree.
        math::Vec3 center;
        math::Quat rot;

        math::Vec4 rotateAt(math::Vec3 pos) const
        {
            math::Vec3 r = rot * pos;
            return math::Vec4{r[0], r[1], r[2], 1.0f};
        }

        aabb getWorldBounds() const;
    };
}