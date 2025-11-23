#include <Coordinate/Euclidean.h>

namespace chai::brew
{
    chai::Vec3 EuclideanSpace::toWorld(const chai::Vec3& local) const
    {
        return local;
    }

    chai::Vec3 EuclideanSpace::toLocal(const chai::Vec3& world) const
    {
        return world;
    }
}