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
        Vec3 center;
        float width;
        float length;
        float height;
    };

    struct BoxCollider
    {
        aabb box;

        // Local rotation is represented by centroid starting point
        // multiplied by quaternion for rotational axis + degree.
        Vec3 center;
        Quat rot;

        Vec4 rotateAt(Vec3 pos) const
        {
            Vec3 r = rot * pos;
            return Vec4{ r[0], r[1], r[2], 1.0f };
        }

        aabb getWorldBounds() const;
    };
}