#pragma once
#include <SceneExport.h>
#include <glm/glm.hpp>
#include <ChaiEngine/CoordinateSpace.h>

namespace chai::cup
{
	enum CameraMode
	{
		PERSPECTIVE,
		ORTHOGRAPHIC
	};

	class SCENE_EXPORT Camera
	{
	public:
		Camera();
		virtual ~Camera();

		void SetPerspective(float fov, float near, float far);
		void SetOrthographic(float left, float right, float bottom, float top, float near, float far);

		void SetPosition(const glm::vec3& pos);
		void setYawPitch(const float yaw, const float pitch);

		//allow setting the direction the camera should point
		void SetDirection(const glm::vec3& dir);
		//or set what the camera should be looking at
		void lookAt(const glm::vec3& targetPos);

		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetProjectionMatrix(float aspect) const;
		glm::mat4 GetViewProjectionMatrix(float aspect) const;

		glm::vec3 getPosition() const;
		glm::vec3 getDirection() const;
		glm::vec3 getRight() const;
		glm::vec3 getUp() const;
		void getYawPitchRoll(float& yaw, float& pitch, float& roll);

		brew::ICoordinateSpace& getCoordinateSpace() const;
		void setCoordinateSpace(std::shared_ptr<brew::ICoordinateSpace> space);

	private:
		glm::vec3 m_position;
		glm::vec3 m_forward;
		glm::vec3 m_up;

		CameraMode m_mode;

		float m_near, m_far;					//clip planes
		float m_fov;							//perspective attributes
		float m_left, m_right, m_top, m_bottom;	//ortho attributes
		float m_yaw, m_pitch, m_roll;

		//SharedViewport m_viewport;
		std::shared_ptr<brew::ICoordinateSpace> m_space;

		void UpdateVectors();
	};
}