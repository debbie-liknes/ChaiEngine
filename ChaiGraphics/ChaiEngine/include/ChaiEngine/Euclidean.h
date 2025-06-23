#pragma once
#include <ChaiEngine/CoordinateSpace.h>

namespace chai::brew
{
    class EuclideanSpace : public ICoordinateSpace {
    public:
        glm::vec3 ToWorld(const glm::vec3& local) const override;
        glm::vec3 ToLocal(const glm::vec3& world) const override;
    };
}