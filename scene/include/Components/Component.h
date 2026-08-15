#pragma once
#include <SceneExport.h>
#include <ChaiReflect.h>
#include <TypeRegistry.h>
#include <TypeInfo.h>
#include <IconsFontAwesome7.h>
#include <Scene/Object.h>

namespace chai::scene
{
    class GameObject;
    class Visitor;

    class SCENE_EXPORT Component : public Object
    {
    public:
        explicit Component(GameObject* owningObject = nullptr);
        virtual ~Component() = default;

        virtual void accept(Visitor* visitor);

        GameObject* getGameObject() const;

    private:
        GameObject* owner_;
    };

} // namespace chai::scene