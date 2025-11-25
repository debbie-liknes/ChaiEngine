#include <ChaiPhysics/Colliders/BoxCollider.h>

namespace chai
{
    bool aabb::operator==(const aabb& rhs) const
    {
        return (
            this->center == rhs.center &&
            this->width == rhs.width &&
            this->length == rhs.length &&
            this->height == rhs.height
        );
    }

    float aabb::getSurfaceArea() const
    {
        return 2.f * (width * width + height * height + length * length);
    }

    aabb aabb::expand(const aabb& box) const
    {
        const float minX = std::min(this->center.x - (this->width / 2.f),
                                    box.center.x - (box.width / 2.f));
        const float minY = std::min(this->center.y - (this->width / 2.f),
                                    box.center.y - (box.width / 2.f));
        const float minZ = std::min(this->center.z - (this->width / 2.f),
                                    box.center.z - (box.width / 2.f));
        const float maxX = std::max(this->center.x + (this->width / 2.f),
                                    box.center.x + (box.width / 2.f));
        const float maxY = std::max(this->center.y + (this->width / 2.f),
                                    box.center.y + (box.width / 2.f));
        const float maxZ = std::max(this->center.z + (this->width / 2.f),
                                    box.center.z + (box.width / 2.f));

        return aabb{
            Vec3((maxX + minX) / 2.f, (maxY + minY) / 2.f, (maxZ + minZ) / 2.f),
            maxX - minX,
            maxY - minY,
            maxZ - minZ
        };
    }

    aabb BoxCollider::getWorldBounds() const
    {
        // the octet cluster of points
        auto lll = center + box.center + Vec3(-box.width / 2.f, -box.height / 2.f, -box.length / 2.f);
        auto rll = center + box.center + Vec3(box.width / 2.f, -box.height / 2.f, -box.length / 2.f);
        auto lul = center + box.center + Vec3(-box.width / 2.f, box.height / 2.f, -box.length / 2.f);
        auto rul = center + box.center + Vec3(box.width / 2.f, box.height / 2.f, -box.length / 2.f);
        auto llr = center + box.center + Vec3(-box.width / 2.f, -box.height / 2.f, box.length / 2.f);
        auto rlr = center + box.center + Vec3(box.width / 2.f, -box.height / 2.f, box.length / 2.f);
        auto lur = center + box.center + Vec3(-box.width / 2.f, box.height / 2.f, box.length / 2.f);
        auto rur = center + box.center + Vec3(box.width / 2.f, box.height / 2.f, box.length / 2.f);

        auto rotatedPoints = {
            rotateAt(lll),
            rotateAt(rll),
            rotateAt(lul),
            rotateAt(rul),
            rotateAt(llr),
            rotateAt(rlr),
            rotateAt(lur),
            rotateAt(rur)
        };

        auto min_x = std::numeric_limits<float>::max();
        auto min_y = std::numeric_limits<float>::max();
        auto min_z = std::numeric_limits<float>::max();
        auto max_x = std::numeric_limits<float>::min();
        auto max_y = std::numeric_limits<float>::min();
        auto max_z = std::numeric_limits<float>::min();
        for (auto&& pt : rotatedPoints)
            if (pt.x < min_x)
                min_x = pt.x;
            else if (pt.x > max_x)
                max_x = pt.x;
        for (auto&& pt : rotatedPoints)
            if (pt.y < min_y)
                min_y = pt.y;
            else if (pt.y > max_y)
                max_y = pt.y;
        for (auto&& pt : rotatedPoints)
            if (pt.z < min_z)
                min_z = pt.z;
            else if (pt.z > max_z)
                max_z = pt.z;

        return aabb{
            Vec3((max_x + min_x) / 2.f, (max_y + min_y) / 2.f, (max_z + min_z) / 2.f),
            max_x - min_x,
            max_y - min_y,
            max_z - min_z
        };
    }
}