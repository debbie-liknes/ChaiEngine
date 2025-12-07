#pragma once
#include "AABB.h"

namespace chai
{
    struct Plane
    {
        Vec3 normal;
        float distance;  // distance from origin

        float distanceTo(const Vec3& point) const
        {
            return dot(normal, point) + distance;
        }
    };

    struct Frustum
    {
        Plane planes[6];

        bool isVisible(const AABB& bounding) const;
        static Frustum fromViewProjection(const Mat4& vp);
        static Plane normalizePlane(const Vec4& p)
        {
            float len = length(Vec3(p.x, p.y, p.z));
            return Plane{
                Vec3(p.x, p.y, p.z) / len,
                p.w / len
            };
        };
    };
}
