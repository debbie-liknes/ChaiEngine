#include <Scene/Camera.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <Coordinate/Euclidean.h>

namespace chai::cup
{
	glm::mat4 Camera::getProjectionMatrix()
	{
		//this is a perspective camera, need to have other types
		return glm::perspective(m_fov, m_aspect, m_nearPlane, m_farPlane);
	}

	glm::mat4 Camera::getViewMatrix()
	{
		return m_viewMatrix;
	}

	void Camera::setAspectRatio(float aspect)
	{
		m_aspect = aspect;
	}
	
	void Camera::setFarPlane(float far)
	{
		m_farPlane = far;
	}

	void Camera::setNearPlan(float near)
	{
		m_nearPlane = near;
	}

	void Camera::setFOV(float fov)
	{
		m_fov = fov;
	}

	void Camera::setViewMatrix(const glm::mat4& viewMatrix)
	{
		//input comes from Transform component
		m_viewMatrix = viewMatrix;
	}
}