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

    double aabb::getSurfaceArea() const
    {
        return 2.0 * (width * width + height * height + length * length);
    }

    aabb aabb::expand(const aabb& box) const
    {
        double min_x = std::min(this->center.x - this->width / 2.0,
            box.center.x - box.width / 2.0);
        double min_y = std::min(this->center.y - this->width / 2.0,
            box.center.y - box.width / 2.0);
        double min_z = std::min(this->center.z - this->width / 2.0,
            box.center.z - box.width / 2.0);
        double max_x = std::max(this->center.x + this->width / 2.0,
            box.center.x + box.width / 2.0);
        double max_y = std::max(this->center.y + this->width / 2.0,
            box.center.y + box.width / 2.0);
        double max_z = std::max(this->center.z + this->width / 2.0,
            box.center.z + box.width / 2.0);

        return aabb{
            glm::vec3((max_x + min_x) / 2.0, (max_y + min_y) / 2.0, (max_z + min_z) / 2.0),
            max_x - min_x,
            max_y - min_y,
            max_z - min_z
        };
    }

    aabb BoxCollider::getWorldBounds() const
    {
        // the octet cluster of points
        auto lll = center + box.center + glm::vec3(-box.width / 2.0, -box.height / 2.0, -box.length / 2.0);
        auto rll = center + box.center + glm::vec3(box.width / 2.0, -box.height / 2.0, -box.length / 2.0);
        auto lul = center + box.center + glm::vec3(-box.width / 2.0, box.height / 2.0, -box.length / 2.0);
        auto rul = center + box.center + glm::vec3(box.width / 2.0, box.height / 2.0, -box.length / 2.0);
        auto llr = center + box.center + glm::vec3(-box.width / 2.0, -box.height / 2.0, box.length / 2.0);
        auto rlr = center + box.center + glm::vec3(box.width / 2.0, -box.height / 2.0, box.length / 2.0);
        auto lur = center + box.center + glm::vec3(-box.width / 2.0, box.height / 2.0, box.length / 2.0);
        auto rur = center + box.center + glm::vec3(box.width / 2.0, box.height / 2.0, box.length / 2.0);

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

        auto min_x = std::numeric_limits<double>::max();
        auto min_y = std::numeric_limits<double>::max();
        auto min_z = std::numeric_limits<double>::max();
        auto max_x = std::numeric_limits<double>::min();
        auto max_y = std::numeric_limits<double>::min();
        auto max_z = std::numeric_limits<double>::min();
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
            glm::vec3((max_x + min_x) / 2.0, (max_y + min_y) / 2.0, (max_z + min_z) / 2.0),
            max_x - min_x,
            max_y - min_y,
            max_z - min_z
        };
    }
}
