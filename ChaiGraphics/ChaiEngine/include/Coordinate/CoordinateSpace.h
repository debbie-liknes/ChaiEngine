#pragma once
#include <glm/glm.hpp>
#include <ChaiMath.h>

namespace chai::brew
{
    class ICoordinateSpace 
    {
    public:
        virtual chai::Vec3 ToWorld(const chai::Vec3& local) const = 0;
        virtual chai::Vec3 ToLocal(const chai::Vec3& world) const = 0;

        virtual ~ICoordinateSpace() = default;
    };
}