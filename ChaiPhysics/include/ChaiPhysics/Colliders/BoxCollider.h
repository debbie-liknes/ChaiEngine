#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace chai
{
    // Axis-aligned Bounding Box
    struct aabb
    {
        bool operator==(const aabb&) const;

        double getSurfaceArea() const;
        aabb expand(const aabb& box) const;

        // The center point in the AABB
        glm::vec3 center;
        double width;
        double length;
        double height;
    };

    struct BoxCollider
    {
        aabb box;

        // Local rotation is represented by centroid starting point
        // multiplied by quaternion for rotational axis + degree.
        glm::vec3 center;
        glm::quat rot;

        glm::vec4 rotateAt(glm::vec3 pos) const
        {
            return rot * glm::vec4(pos, 1.) * glm::inverse(rot);
        }

        aabb getWorldBounds() const;
    };
}