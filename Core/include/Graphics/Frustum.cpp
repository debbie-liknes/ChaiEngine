#include "Frustum.h"

namespace chai
{
    bool Frustum::isVisible(const AABB& box) const
    {
        for (int i = 0; i < 6; i++) {
            Vec3 pVertex = box.GetPositiveVertex(planes[i].normal);
            float dist = planes[i].distanceTo(pVertex);
            if (dist < 0) {
                return false;
            }
        }
        return true;
    }

    Frustum Frustum::fromViewProjection(const Mat4& vp)
    {
        Frustum f;

        Vec4 row0(vp[0][0], vp[1][0], vp[2][0], vp[3][0]);
        Vec4 row1(vp[0][1], vp[1][1], vp[2][1], vp[3][1]);
        Vec4 row2(vp[0][2], vp[1][2], vp[2][2], vp[3][2]);
        Vec4 row3(vp[0][3], vp[1][3], vp[2][3], vp[3][3]);

        f.planes[0] = normalizePlane(row3 + row0);  // Left
        f.planes[1] = normalizePlane(row3 - row0);  // Right
        f.planes[2] = normalizePlane(row3 + row1);  // Bottom
        f.planes[3] = normalizePlane(row3 - row1);  // Top
        f.planes[4] = normalizePlane(row3 + row2);  // Near
        f.planes[5] = normalizePlane(row3 - row2);  // Far

        return f;
    }
}