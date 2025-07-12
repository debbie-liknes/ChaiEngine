#pragma once
#include <SceneExport.h>
#include <Components/ComponentBase.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <ChaiMath.h>

namespace chai::cup
{
	class GameObject;
	class SCENE_EXPORT TransformComponent : public Component
	{
	public:
		explicit TransformComponent(GameObject* owner = nullptr);
		~TransformComponent() override = default;
		Mat4 getLocalMatrix() const;
		Mat4 getWorldMatrix() const;

		void setPosition(chai::Vec3 newPos);

		Vec3 up() const;
		Vec3 forward() const;
		Vec3 right() const;

		Vec3 getWorldPosition() const;
		Quat getWorldRotation() const;

		void lookAt(const Vec3& target, const Vec3& worldUp);
	private:

		TransformComponent* m_parent{ nullptr };
		Vec3 m_position{ 0.0f, 0.0f, 0.0f };
		Quat m_rotation{ 1.0f, 0.0f, 0.0f, 0.0f }; // w, x, y, z
		Vec3 m_scale{ 1.0f, 1.0f, 1.0f };
	};
}