#pragma once
#include <glm/glm.hpp>

namespace chai::brew
{
    class ICoordinateSpace {
    public:
        virtual glm::vec3 ToWorld(const glm::vec3& local) const = 0;
        virtual glm::vec3 ToLocal(const glm::vec3& world) const = 0;
    };
}