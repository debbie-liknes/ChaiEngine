#pragma once
#include <SceneExport.h>
#include <Components/Component.h>
#include <Scene/Camera.h>
#include <memory>
#include <ChaiMath.h>
#include <Scene/IUpdatable.h>
#include <Scene/GameObject.h>

namespace chai::scene
{
    class GameObject;
    class TransformComponent;

    class SCENE_EXPORT CameraComponent : public Component, public IUpdatable
    {
    public:
        explicit CameraComponent(GameObject* owner = nullptr);
        ~CameraComponent() override = default;

        [[nodiscard]] const Camera& getCamera() const { return cam_; }

        void setAspectRatio(float aspect);
        void setFarPlane(float far);
        void setNearPlane(float near);
        void setFOV(float fov);

    private:
        Camera cam_;
    };

    CHAI_REFLECT(CameraComponent, "CameraComponent")
    {
        CHAI_ICON(ICON_FA_VIDEO);
    }
}