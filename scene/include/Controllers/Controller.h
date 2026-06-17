#pragma once
#include <SceneExport.h>
#include <Core/IInput.h>

namespace chai::scene
{
    class GameObject;

    class SCENE_EXPORT IController
    {
    public:
        virtual ~IController() = default;

        virtual void update(const UpdateContext&) = 0;
        virtual void setEnabled(bool enabled) = 0;
        virtual bool isEnabled() const = 0;

        //maybe optional
        virtual const char* getControllerType() const = 0;

    protected:
        chai::scene::GameObject* gameObject_ = nullptr;
        bool enabled_ = true;

        explicit IController(chai::scene::GameObject* obj) : gameObject_(obj) {}
    };
}