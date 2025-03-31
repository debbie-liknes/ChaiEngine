#include <ChaiEngine/Euclidean.h>

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

    glm::mat4 EuclideanSpace::GetViewMatrix(const GPUCamera& cam) const
    {
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
		view[3][0] = -glm::dot(cameraRight, cam.position);
		view[3][1] = -glm::dot(cam.up, cam.position);
		view[3][2] = glm::dot(cam.forward, cam.position); // Note the + sign here

		return view;
    }
}