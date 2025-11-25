#include <Coordinate/Spherical.h>

namespace chai::brew
{
    chai::Vec3 sphericalToCartesian(float r, float theta, float phi)
    {
        return {
            r * (float)cos(phi) * (float)cos(theta),
            r * (float)sin(phi),
            r * (float)cos(phi) * (float)sin(theta)
        };
    }

    chai::Vec3 SphericalSpace::toWorld(const chai::Vec3& local) const
    {
        float r = local.x;
        float theta = local.y;
        float phi = local.z;

        return sphericalToCartesian(r, theta, phi);
    }

    chai::Vec3 SphericalSpace::toLocal(const chai::Vec3& world) const
    {
        return world;
    }
}