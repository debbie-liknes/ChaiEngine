#pragma once
#include <SceneExport.h>
#include <Controllers/Controller.h>
#include <Scene/IUpdatable.h>

namespace chai::scene
{
    class GameObject;

    class SCENE_EXPORT SpinController : public IController, public IUpdatable
    {
    public:
        SpinController(chai::scene::GameObject* obj);
        virtual ~SpinController() = default;

        void update(const UpdateContext&) override;
        void setEnabled(bool enabled) override;
        bool isEnabled() const override;

        // maybe optional
        const char* getControllerType() const override { return "SpinController"; }

    private:
        float rotationSpeed_ = 90.f;

    };
} // namespace chai::scene