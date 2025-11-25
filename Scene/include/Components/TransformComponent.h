#pragma once
#include <SceneExport.h>
#include <Components/ComponentBase.h>
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
        void setRotationEuler(chai::Vec3 newRot);
        void setScale(chai::Vec3 newScale);

        Vec3 up() const;
        Vec3 forward() const;
        Vec3 right() const;

        Vec3 getWorldPosition() const;
        Quat getWorldRotation() const;

        void lookAt(const Vec3& target, const Vec3& worldUp);

    private:
        TransformComponent* m_parent{nullptr};
        Vec3 m_position{0.0f, 0.0f, 0.0f};
        Quat m_rotation{0.0f, 0.0f, 0.0f, 0.0f}; // w, x, y, z
        Vec3 m_scale{1.0f, 1.0f, 1.0f};
    };
}