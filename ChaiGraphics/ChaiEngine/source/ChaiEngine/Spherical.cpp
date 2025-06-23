#include <ChaiEngine/Spherical.h>

namespace chai::brew
{
    glm::vec3 sphericalToCartesian(float r, float theta, float phi) {
        return {
            r * cos(phi) * cos(theta),
            r * sin(phi),
            r * cos(phi) * sin(theta)
        };
    }

    glm::vec3 SphericalSpace::ToWorld(const glm::vec3& local) const
    {
        float r = local.x;
        float theta = local.y;
        float phi = local.z;

        return sphericalToCartesian(r, theta, phi);
    }

    glm::vec3 SphericalSpace::ToLocal(const glm::vec3& world) const
    {
        return world;
    }
}