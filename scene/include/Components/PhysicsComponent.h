#pragma once
#include <ChaiMath.h>
#include <ChaiPhysics/Colliders/ColliderBase.h>
#include <Components/ComponentBase.h>

namespace chai::cup
{
    class GameObject;

    class PhysicsComponent : public Component
    {
    public:
        explicit PhysicsComponent(GameObject* owner = nullptr);

        void update(double deltaTime) override;

        ColliderBase* collider = nullptr;

        float mass = 1.f;
        float drag = 1.f;
        float gravity = 0.f;
    };
} // namespace chai::cup