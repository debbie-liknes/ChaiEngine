#pragma once
#include <SceneExport.h>
#include <Updatable.h>

namespace chai::scene
{
    class GameObject;

    class SCENE_EXPORT Component : public IUpdatable
    {
    public:
        explicit Component(GameObject* owningObject = nullptr) : owningObject_(owningObject)
        {
        }

        virtual ~Component() = default;

        GameObject* getGameObject() const { return owningObject_; }

    private:
        GameObject* owningObject_ = nullptr;
    };
}