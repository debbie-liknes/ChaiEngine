#pragma once
#include <Controllers/Controller.h>
#include <Scene/GameObject.h>

namespace chai::scene
{
    class FlyCameraController : public IController
    {
    public:
        FlyCameraController(chai::scene::GameObject* obj);
        void update(const UpdateContext& ctx) override;

        void setEnabled(bool enabled) override;
        bool isEnabled() const override;

        // maybe optional
        const char* getControllerType() const override;

    private:
        float yaw_ = 0.f;             // radians, about +Y
        float pitch_ = 0.f;           // radians, about +X
        float speed_ = 3.f;           // units/sec
        float sensitivity_ = 0.0025f; // radians per pixel
    };

}