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

    math::Vec2 InputHandler::scrollDelta() const
    {
        return math::Vec2{scrollX, scrollY};
    }

    void InputHandler::setCursorMode(CursorMode mode)
    {

    }

    void InputHandler::newFrame()
    {
        deltaX = 0.f;
        deltaY = 0.f;
        scrollX = 0.f;
        scrollY = 0.f;
        typedChars_.clear();

        // TODO: This needs to go somewhere else, but for now it will do
        bool dragButtonDown =
            mouseDown(MouseButton::Right);

        if (dragButtonDown && !wasDragButtonDown_ && hoveredCamera_ != 0) {
            capturedCamera_ =
                hoveredCamera_;
        }
        if (!dragButtonDown) {
            capturedCamera_ = 0;
        }
        wasDragButtonDown_ = dragButtonDown;
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

    void InputHandler::setHoveredCamera(uint64_t cameraId) 
    {
        hoveredCamera_ = cameraId;
    }

    uint64_t InputHandler::getHoveredCamera() const
    {
        return hoveredCamera_;
    }

    uint64_t InputHandler::getActiveCamera() const
    {
        return capturedCamera_ != 0 ? capturedCamera_ : hoveredCamera_;
    }

    const std::vector<unsigned int>& InputHandler::getTypedCharactersThisFrame() const
    {
        return typedChars_;
    }

    void InputHandler::updateCharInput(const CharEvent& input)
    {
        typedChars_.push_back(input.input);
    }

    void InputHandler::updateScrollInput(const MouseScrollEvent& input)
    {
        scrollX = input.x;
        scrollY = input.y;
    }
}

namespace chai
{
    Key toChaiKey(int glfwKey)
    {
        if (glfwKey == GLFW_KEY_A)
            return Key::A;
        if (glfwKey == GLFW_KEY_B)
            return Key::B;
        if (glfwKey == GLFW_KEY_C)
            return Key::C;
        if (glfwKey == GLFW_KEY_D)
            return Key::D;
        if (glfwKey == GLFW_KEY_E)
            return Key::E;
        if (glfwKey == GLFW_KEY_F)
            return Key::F;
        if (glfwKey == GLFW_KEY_G)
            return Key::G;
        if (glfwKey == GLFW_KEY_H)
            return Key::H;
        if (glfwKey == GLFW_KEY_I)
            return Key::I;
        if (glfwKey == GLFW_KEY_J)
            return Key::J;
        if (glfwKey == GLFW_KEY_K)
            return Key::K;
        if (glfwKey == GLFW_KEY_L)
            return Key::L;
        if (glfwKey == GLFW_KEY_M)
            return Key::M;
        if (glfwKey == GLFW_KEY_N)
            return Key::N;
        if (glfwKey == GLFW_KEY_O)
            return Key::O;
        if (glfwKey == GLFW_KEY_P)
            return Key::P;
        if (glfwKey == GLFW_KEY_Q)
            return Key::Q;
        if (glfwKey == GLFW_KEY_R)
            return Key::R;
        if (glfwKey == GLFW_KEY_S)
            return Key::S;
        if (glfwKey == GLFW_KEY_T)
            return Key::T;
        if (glfwKey == GLFW_KEY_U)
            return Key::U;
        if (glfwKey == GLFW_KEY_V)
            return Key::V;
        if (glfwKey == GLFW_KEY_W)
            return Key::W;
        if (glfwKey == GLFW_KEY_X)
            return Key::X;
        if (glfwKey == GLFW_KEY_Y)
            return Key::Y;
        if (glfwKey == GLFW_KEY_Z)
            return Key::Z;
        if (glfwKey == GLFW_KEY_SPACE)
            return Key::Space;
        if (glfwKey == GLFW_KEY_LEFT_SHIFT)
            return Key::LeftShift;
        if (glfwKey == GLFW_KEY_LEFT_CONTROL)
            return Key::LeftCtrl;
        if (glfwKey == GLFW_KEY_UP)
            return Key::Up;
        if (glfwKey == GLFW_KEY_DOWN)
            return Key::Down;
        if (glfwKey == GLFW_KEY_LEFT)
            return Key::Left;
        if (glfwKey == GLFW_KEY_RIGHT)
            return Key::Right;
        if (glfwKey == GLFW_KEY_ESCAPE)
            return Key::Escape;
        if (glfwKey == GLFW_KEY_BACKSPACE)
            return Key::Backspace;

        return Key::Invalid;
    }
}