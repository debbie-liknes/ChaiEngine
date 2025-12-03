#include <Graphics/AABB.h>

namespace chai
{
    Vec3 AABB::GetPositiveVertex(const Vec3& normal) const {
        return Vec3(
            normal.x >= 0 ? max.x : min.x,
            normal.y >= 0 ? max.y : min.y,
            normal.z >= 0 ? max.z : min.z
        );
    }

    Vec3 AABB::GetNegativeVertex(const Vec3& normal) const {
        return Vec3(
            normal.x >= 0 ? min.x : max.x,
            normal.y >= 0 ? min.y : max.y,
            normal.z >= 0 ? min.z : max.z
        );
    }
}