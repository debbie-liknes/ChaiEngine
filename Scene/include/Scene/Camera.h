#pragma once
#include <SceneExport.h>
#include <glm/glm.hpp>
#include <Scene/ICamera.h>
#include <ChaiEngine/CoordinateSpace.h>
#include <glm/gtc/matrix_transform.hpp>

namespace chai::cup
{
	class SCENE_EXPORT Camera : public chai::ICamera
	{
	public:
		Camera() {}
		~Camera() {}

		glm::mat4 getProjectionMatrix() override;
		glm::mat4 getViewMatrix() override;
		void setViewMatrix(const glm::mat4& viewMatrix) override;

		void setAspectRatio(float aspect) override;
		void setFarPlane(float far) override;
		void setNearPlan(float near) override;
		void setFOV(float fov) override;

	private:
		float m_aspect = 0.0;
		float m_fov = 45.0f; // degrees
		float m_nearPlane = 0.1f;
		float m_farPlane = 100.0f;

		glm::mat4 m_viewMatrix{ 1.f };

	};
}