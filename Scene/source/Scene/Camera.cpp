#include <Scene/Camera.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace chai::cup
{
	Camera::Camera() : m_position(glm::vec3{0.f}), m_forward(glm::vec3{0.f}), m_up(glm::vec3{0.f, 1.f, 0.f}),
		m_mode(CameraMode::PERSPECTIVE), m_fov(0.f), m_near(0.f), m_far(0.f),
		m_left(0.f), m_right(0.f), m_bottom(0.f), m_top(0.f)
	{

	}

	Camera::~Camera()
	{

	}

	void Camera::SetPerspective(float fov, float near, float far)
	{
		m_mode = CameraMode::PERSPECTIVE;
		m_fov = fov;
		m_near = near;
		m_far = far;
	}

	void Camera::SetOrthographic(float left, float right, float bottom, float top, float near, float far)
	{
		m_mode = CameraMode::ORTHOGRAPHIC;
		m_left = left;
		m_right = right;
		m_bottom = bottom;
		m_top = top;
		m_near = near;
		m_far = far;
	}

	void Camera::SetPosition(const glm::vec3& pos)
	{
		m_position = pos;
	}

	void Camera::SetDirection(const glm::vec3& dir)
	{
		m_forward = glm::normalize(dir);
	}

	void Camera::lookAt(const glm::vec3& targetPos)
	{
		SetDirection(glm::normalize(targetPos - m_position));
	}

	//world space to view space
	glm::mat4 Camera::GetViewMatrix() const
	{
		glm::vec3 cameraRight = glm::normalize(glm::cross(m_forward, m_up));
		glm::mat4 view(1.0f);

		// Set the rotation part (transposed because view = inverse(camera transform))
		view[0][0] = cameraRight.x;
		view[1][0] = cameraRight.y;
		view[2][0] = cameraRight.z;

		view[0][1] = m_up.x;
		view[1][1] = m_up.y;
		view[2][1] = m_up.z;

		view[0][2] = -m_forward.x;
		view[1][2] = -m_forward.y;
		view[2][2] = -m_forward.z;

		// Set the translation part
		view[3][0] = -glm::dot(cameraRight, m_position);
		view[3][1] = -glm::dot(m_up, m_position);
		view[3][2] = glm::dot(m_forward, m_position); // Note the + sign here

		return view;
	}

	//view space to clip space
	glm::mat4 Camera::GetProjectionMatrix(float aspect) const
	{
		if (m_mode == CameraMode::ORTHOGRAPHIC)
		{
			return glm::ortho(m_left, m_right, m_bottom, m_top);
		}
		return glm::perspective(m_fov, aspect, m_near, m_far);
	}

	//world space to clip space
	glm::mat4 Camera::GetViewProjectionMatrix(float aspect) const
	{
		return GetProjectionMatrix(aspect) * GetViewMatrix();
	}

	glm::vec3 Camera::getPosition() const
	{
		return m_position;
	}

	glm::vec3 Camera::getDirection() const
	{
		return m_forward;
	}

	glm::vec3 Camera::getRight() const
	{
		return glm::normalize(glm::cross(m_forward, m_up));
	}
}