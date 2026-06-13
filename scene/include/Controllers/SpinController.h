#pragma once
#include <SceneExport.h>
#include <Controllers/Controller.h>

namespace chai::scene
{
    class GameObject;

    class SCENE_EXPORT SpinController : public IController
    {
    public:
        SpinController(chai::scene::GameObject* obj);
        virtual ~SpinController() = default;

        void update(float deltaTime);
        void setEnabled(bool enabled);
        bool isEnabled() const;

        // maybe optional
        const char* getControllerType() const override { return "SpinController"; }

    private:
        float rotationSpeed_ = 90.f;

    };
} // namespace chai::scene