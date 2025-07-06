#include <Coordinate/Euclidean.h>

namespace chai::brew
{
    chai::Vec3 EuclideanSpace::ToWorld(const chai::Vec3& local) const
    {
        return local;
    }

    chai::Vec3 EuclideanSpace::ToLocal(const chai::Vec3& world) const
    {
        return world;
    }
}