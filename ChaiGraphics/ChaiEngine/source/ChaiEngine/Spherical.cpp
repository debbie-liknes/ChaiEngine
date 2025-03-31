#include <ChaiEngine/Spherical.h>

namespace chai::brew
{
    glm::vec3 sphericalToCartesian(float r, float theta, float phi) {
        return {
            r * cos(phi) * cos(theta),
            r * sin(phi),
            r * cos(phi) * sin(theta)
        };
    }

    glm::vec3 SphericalSpace::ToWorld(const glm::vec3& local) const
    {
        float r = local.x;
        float theta = local.y;
        float phi = local.z;

        return sphericalToCartesian(r, theta, phi);
    }

    glm::vec3 SphericalSpace::ToLocal(const glm::vec3& world) const
    {
        return world;
    }

    glm::mat4 SphericalSpace::GetViewMatrix(const GPUCamera& cam) const
    {
        glm::vec3 pos = sphericalToCartesian(cam.position.x, cam.position.y, cam.position.z);

        glm::vec3 cameraRight = glm::normalize(glm::cross(cam.forward, cam.up));
        glm::mat4 view(1.0f);

        // Set the rotation part (transposed because view = inverse(camera transform))
        view[0][0] = cameraRight.x;
        view[1][0] = cameraRight.y;
        view[2][0] = cameraRight.z;

        view[0][1] = cam.up.x;
        view[1][1] = cam.up.y;
        view[2][1] = cam.up.z;

        view[0][2] = -cam.forward.x;
        view[1][2] = -cam.forward.y;
        view[2][2] = -cam.forward.z;

        // Set the translation part
        view[3][0] = -glm::dot(cameraRight, pos);
        view[3][1] = -glm::dot(cam.up, pos);
        view[3][2] = glm::dot(cam.forward, pos); // Note the + sign here

        return view;
    }
}