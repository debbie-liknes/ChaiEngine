#pragma once
#include <Coordinate/CoordinateSpace.h>

namespace chai::brew
{
    class EuclideanSpace : public ICoordinateSpace 
    {
    public:
        chai::Vec3 ToWorld(const chai::Vec3& local) const override;
        chai::Vec3 ToLocal(const chai::Vec3& world) const override;
    };
}