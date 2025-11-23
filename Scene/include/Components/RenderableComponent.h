#pragma once
#include <SceneExport.h>
#include <Components/ComponentBase.h>

namespace chai::cup
{
    class SCENE_EXPORT RenderableComponent : public Component
    {
    public:
        explicit RenderableComponent(GameObject* owner = nullptr) : Component(owner)
        {
        }

    protected:
    };
}