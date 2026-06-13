#pragma once
#include <SceneExport.h>
#include <IComponent.h>
#include <Scene/Camera.h>
#include <memory>
#include <ChaiMath.h>
#include <Updatable.h>
#include <Scene/GameObject.h>

namespace chai::scene
{
    class GameObject;
    class TransformComponent;

    class SCENE_EXPORT CameraComponent : public IComponent, public IUpdatable
    {
    public:
        explicit CameraComponent(GameObject* owner = nullptr);
        ~CameraComponent() override = default;

        void extract(gfx::FrameRenderData & frame) const override;

        void setAspectRatio(float aspect);
        void setFarPlane(float far);
        void setNearPlane(float near);
        void setFOV(float fov);

    private:
        Camera cam_;
    };
}