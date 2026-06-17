#include <Components/Component.h>
#include <Scene/GameObject.h>

namespace chai::scene
{
    Component::Component(GameObject* owningObject) : owner_(owningObject)
    {

    }

    GameObject* Component::getGameObject() const
    {
        return owner_;
    }
}