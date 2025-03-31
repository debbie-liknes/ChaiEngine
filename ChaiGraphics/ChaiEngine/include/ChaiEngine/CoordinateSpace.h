#pragma once
#include <glm/glm.hpp>
#include <ChaiEngine/RenderFrame.h>

namespace chai::brew
{
    class ICoordinateSpace {
    public:
        virtual glm::vec3 ToWorld(const glm::vec3& local) const = 0;
        virtual glm::vec3 ToLocal(const glm::vec3& world) const = 0;
        virtual glm::mat4 GetViewMatrix(const GPUCamera& cam) const = 0;
    };
}