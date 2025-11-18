#pragma once

namespace chai::cup
{
    class GameObject;
    class IController
    {
    public:
        virtual ~IController() = default;

        virtual void update(double deltaTime) = 0;
        virtual void setEnabled(bool enabled) = 0;
        virtual bool isEnabled() const = 0;

        //maybe optional
        virtual const char* getControllerType() const = 0;

    protected:
        chai::cup::GameObject* gameObject = nullptr;
        bool enabled = true;

        explicit IController(chai::cup::GameObject* obj) : gameObject(obj) {}
    };
}