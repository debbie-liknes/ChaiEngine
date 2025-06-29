#pragma once
#include <SceneExport.h>
#include <Scene/ComponentBase.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace chai::cup
{
	class GameObject;
	class SCENE_EXPORT TransformComponent : public Component
	{
	public:
		TransformComponent(GameObject* owner = nullptr);
		~TransformComponent() = default;
		glm::mat4 getLocalMatrix() const;
		glm::mat4 getWorldMatrix() const;

		void setPosition(glm::vec3 newPos);

		glm::vec3 up() const;
		glm::vec3 forward() const;
		glm::vec3 right() const;

		glm::vec3 getWorldPosition() const;
		glm::quat getWorldRotation() const;

		void lookAt(const glm::vec3& target, const glm::vec3& worldUp);
	private:

		TransformComponent* m_parent{ nullptr };
		glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
		glm::quat m_rotation{ 1.0f, 0.0f, 0.0f, 0.0f }; // w, x, y, z
		glm::vec3 m_scale{ 1.0f, 1.0f, 1.0f };
	};
}