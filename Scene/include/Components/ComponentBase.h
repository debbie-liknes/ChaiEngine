#pragma once
#include <SceneExport.h>
#include <Updatable.h>

namespace chai::scene
{
    class GameObject;

    class SCENE_EXPORT Component : public IUpdatable
    {
    public:
        explicit Component(GameObject* owningObject = nullptr) : m_owningObject(owningObject)
        {
        }

        virtual ~Component() = default;

        virtual void update(double deltaTime) override {}

        GameObject* getGameObject() const { return m_owningObject; }

    private:
        GameObject* m_owningObject = nullptr;
    };
}