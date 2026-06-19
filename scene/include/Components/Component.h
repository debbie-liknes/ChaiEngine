#pragma once
#include <SceneExport.h>

namespace chai::scene
{
    class GameObject;

    class SCENE_EXPORT Component
    {
    public:
        explicit Component(GameObject* owningObject = nullptr);
        virtual ~Component() = default;

        GameObject* getGameObject() const;

    private:
        GameObject* owner_;
    };
} // namespace chai::scene