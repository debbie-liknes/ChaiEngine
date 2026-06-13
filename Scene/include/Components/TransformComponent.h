#pragma once
#include <SceneExport.h>
#include <ChaiMath.h>
#include <Updatable.h>
#include <IComponent.h>

namespace chai::scene
{
    class GameObject;

    class SCENE_EXPORT TransformComponent : public IComponent, public IUpdatable
    {
    public:
        explicit TransformComponent(GameObject* owner = nullptr);
        ~TransformComponent() override = default;
        math::Mat4 getLocalMatrix() const;
        math::Mat4 getWorldMatrix() const;

        void setLocalMatrix(const math::Mat4& matrix);

        void setPosition(math::Vec3 newPos);
        void setRotationEuler(math::Vec3 newRot);
        void setRotation(const math::Quat& rot)
        { 
            rotation_ = rot;
        }
        void setScale(math::Vec3 newScale);

        math::Vec3 up() const;
        math::Vec3 forward() const;
        math::Vec3 right() const;

        math::Vec3 getWorldPosition() const;

        math::Quat getWorldRotation() const;
        math::Quat getLocalRotation() const;

        void lookAt(const math::Vec3& target, const math::Vec3& worldUp);

    private:
        math::Vec3 position_{0.0f, 0.0f, 0.0f};
        math::Quat rotation_;
        math::Vec3 scale_{1.0f, 1.0f, 1.0f};
    };
}