#pragma once
#include <SceneExport.h>
#include <Components/ComponentBase.h>
#include <Scene/Camera.h>
#include <memory>

namespace chai::cup
{
	class GameObject;
	class TransformComponent;
	class SCENE_EXPORT CameraComponent : public Component
	{
	public:
		explicit CameraComponent(GameObject* owner = nullptr);
		~CameraComponent() override = default;

		glm::mat4 getViewMatrix() const;
		glm::mat4 getProjectionMatrix() const { return m_camera->getProjectionMatrix(); }
		ICamera* getCamera() { return m_camera.get(); }

		void update(double deltaTime) override;

	private:
		std::unique_ptr<ICamera> m_camera;

		void updateViewMatrix(TransformComponent* transform);
	};
}