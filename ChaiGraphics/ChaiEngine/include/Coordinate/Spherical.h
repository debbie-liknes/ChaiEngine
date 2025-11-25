#pragma once
#include <Coordinate/CoordinateSpace.h>

namespace chai::brew
{
    class SphericalSpace : public ICoordinateSpace
    {
    public:
        chai::Vec3 toWorld(const chai::Vec3& local) const override;
        chai::Vec3 toLocal(const chai::Vec3& world) const override;
    };
}