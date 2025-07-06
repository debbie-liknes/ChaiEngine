#include <Coordinate/Spherical.h>

namespace chai::brew
{
    chai::Vec3 sphericalToCartesian(float r, float theta, float phi) {
        return {
            r * cos(phi) * cos(theta),
            r * sin(phi),
            r * cos(phi) * sin(theta)
        };
    }

    chai::Vec3 SphericalSpace::ToWorld(const chai::Vec3& local) const
    {
        float r = local.x;
        float theta = local.y;
        float phi = local.z;

        return sphericalToCartesian(r, theta, phi);
    }

    chai::Vec3 SphericalSpace::ToLocal(const chai::Vec3& world) const
    {
        return world;
    }
}