#include <Coordinate/Euclidean.h>

namespace chai::brew
{
    glm::vec3 EuclideanSpace::ToWorld(const glm::vec3& local) const
    {
        return local;
    }

    glm::vec3 EuclideanSpace::ToLocal(const glm::vec3& world) const
    {
        return world;
    }
}