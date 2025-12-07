#pragma once
#include <SceneExport.h>
#include <Core/Updatable.h>

namespace chai::cup
{
    class GameObject;

    class SCENE_EXPORT Component : public IUpdatable
    {
    public:
        explicit Component(GameObject* owningObject = nullptr) : m_owningObject(owningObject)
        {
        }

        ~Component() override = default;

        virtual void update(double deltaTime) override
        {
        }

        GameObject* getGameObject() const { return m_owningObject; }

        void setDirty(bool dirty = true){ m_dirty = dirty; }
        bool dirty() { return m_dirty; }

    private:
        GameObject* m_owningObject = nullptr;
        bool m_dirty = false;
    };
}