#pragma once
#include <SceneExport.h>
#include <Components/ComponentBase.h>
#include <ChaiMath.h>

namespace chai::scene
{
    class GameObject;

    class SCENE_EXPORT TransformComponent : public Component
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
            m_rotation = rot;
        }
        void setScale(math::Vec3 newScale);

        math::Vec3 up() const;
        math::Vec3 forward() const;
        math::Vec3 right() const;

        math::Vec3 getWorldPosition() const;
        math::Quat getWorldRotation() const;

        void lookAt(const math::Vec3& target, const math::Vec3& worldUp);

    private:
        math::Vec3 m_position{0.0f, 0.0f, 0.0f};
        math::Quat m_rotation;
        math::Vec3 m_scale{1.0f, 1.0f, 1.0f};
    };
}