#pragma once
#include <SceneExport.h>
#include <ChaiMath.h>
#include <Components/Component.h>

namespace chai::scene
{
    class GameObject;

    class TransformComponent : public Component
    {
    public:
        CHAI_OBJECT(TransformComponent)

        explicit TransformComponent(GameObject* owner = nullptr);
        ~TransformComponent() override = default;

        void setPosition(math::Vec3 newPos);
        void setRotation(const math::Quat& rot);
        void setRotationEuler(const math::Vec3& degrees);
        void setScale(math::Vec3 newScale);

        math::Vec3 up() const;
        math::Vec3 forward() const;
        math::Vec3 right() const;

        math::Vec3 getScale() const;

        //world
        math::Vec3 getWorldPosition() const;
        math::Quat getWorldRotation() const;
        math::Mat4 getWorldMatrix() const;

        //local
        math::Vec3 getLocalPosition() const;
        math::Quat getLocalRotation() const;
        math::Vec3 getLocalRotationEuler() const;
        math::Mat4 getLocalMatrix() const;

        void lookAt(const math::Vec3& target, const math::Vec3& worldUp);

    private:
        math::Vec3 position_{0.0f, 0.0f, 0.0f};
        math::Quat rotation_;
        math::Vec3 scale_{1.0f, 1.0f, 1.0f};
    };

    CHAI_REFLECT(TransformComponent, "TransformComponent") 
    {
        CHAI_ICON(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT);
        CHAI_PROPERTY("Position", getLocalPosition, setPosition);
        CHAI_PROPERTY("Rotation", getLocalRotationEuler, setRotationEuler);
        CHAI_PROPERTY("Scale", getScale, setScale);
    }
}