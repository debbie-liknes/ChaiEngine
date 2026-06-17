#include "Input.h"
#include <GLFW/glfw3.h>

namespace chai
{
    bool InputHandler::keyDown(Key k) const
    {
        return keys_.contains(k);
    }

    bool InputHandler::mouseDown(MouseButton b) const
    {
        return mouseButtons_.contains(b);
    }

    math::Vec2 InputHandler::mousePosition() const
    {
        return math::Vec2{mouseX, mouseY};
    }

    math::Vec2 InputHandler::mouseDelta() const
    {
        return math::Vec2{deltaX, deltaY};
    }

    float InputHandler::scrollDelta() const
    {
        return 0.f;
    }

    void InputHandler::setCursorMode(CursorMode mode)
    {

    }

    void InputHandler::newFrame()
    {
        deltaX = 0.f;
        deltaY = 0.f;
    }

    void InputHandler::updateKeyPress(const KeyEvent& key)
    {
        if (key.type == InputEventType::KeyPress) {
            keys_.insert(key.key);
        }
        else if (key.type == InputEventType::KeyRelease) {
            keys_.erase(key.key);
        }
    }

    void InputHandler::updateMousePress(const MouseButtonEvent& mouse)
    {
        if (mouse.type == InputEventType::MouseButtonDown) {
            mouseButtons_.insert(mouse.mouse);
        } else if (mouse.type == InputEventType::MouseButtonUp) {
            mouseButtons_.erase(mouse.mouse);
        }
    }

    void InputHandler::updateMouseMove(const MouseMoveEvent& mouse)
    {
        if (mouse.type != InputEventType::MouseMove)
            return;
        deltaX = mouse.x - mouseX;
        deltaY = mouse.y - mouseY;
        mouseX = mouse.x;
        mouseY = mouse.y;
    }
}

namespace chai
{
    Key toChaiKey(int glfwKey)
    {
        if (glfwKey == GLFW_KEY_A)
            return Key::A;
        if (glfwKey == GLFW_KEY_E)
            return Key::E;
        if (glfwKey == GLFW_KEY_D)
            return Key::D;
        if (glfwKey == GLFW_KEY_Q)
            return Key::Q;
        if (glfwKey == GLFW_KEY_S)
            return Key::S;
        if (glfwKey == GLFW_KEY_W)
            return Key::W;

        return Key::Invalid;
    }
}