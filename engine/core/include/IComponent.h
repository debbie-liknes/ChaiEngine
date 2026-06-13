#pragma once

namespace chai::scene
{
    class GameObject;

    class IComponent
    {
    public:
        explicit IComponent(GameObject* owningObject = nullptr) : owningObject_(owningObject) {}
        virtual ~IComponent() = default;

        GameObject* getGameObject() const { return owningObject_; }

    private:
        GameObject* owningObject_ = nullptr;
    };
}