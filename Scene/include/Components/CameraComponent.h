#pragma once
#include <SceneExport.h>
#include <Components/ComponentBase.h>
#include <Scene/Camera.h>
#include <memory>
#include <ChaiMath.h>

namespace chai::cup
{
    class GameObject;
    class TransformComponent;

    class SCENE_EXPORT CameraComponent : public Component
    {
    public:
        explicit CameraComponent(GameObject* owner = nullptr);
        ~CameraComponent() override = default;

        Mat4 getViewMatrix() const;
        Mat4 getProjectionMatrix() const { return m_camera->getProjectionMatrix(); }
        ICamera* getCamera() { return m_camera.get(); }

        void update(double deltaTime) override;

    private:
        std::unique_ptr<ICamera> m_camera;

        void updateViewMatrix(TransformComponent* transform);
    };
}