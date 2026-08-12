#include <Components/Component.h>
#include <Scene/GameObject.h>
#include <Visitors/Visitor.h>

namespace chai::scene
{
    Component::Component(GameObject* owningObject) : owner_(owningObject)
    {

    }

    void Component::accept(Visitor* visitor)
    {
        visitor->visit(this);
    }

    GameObject* Component::getGameObject() const
    {
        return owner_;
    }
}
