/**
 * @file Input.h
 */
#pragma once
#include <Core/IInput.h>
#include <set>

namespace chai
{
    /**
     * @brief GLFW input handling
     */
	class InputHandler : public IInput
	{
    public:
        InputHandler() = default;
        ~InputHandler() = default;

        [[nodiscard]] bool keyDown(Key k) const override;
        [[nodiscard]] bool mouseDown(MouseButton b) const override;

        [[nodiscard]] math::Vec2 mousePosition() const override;
        [[nodiscard]] math::Vec2 mouseDelta() const override;
        [[nodiscard]] float scrollDelta() const override;

        void setCursorMode(CursorMode mode) override;

        void newFrame() override;

        void updateKeyPress(const KeyEvent& key);
        void updateMousePress(const MouseButtonEvent& mouse);
        void updateMouseMove(const MouseMoveEvent& mouse);

    private:
        std::set<Key> keys_;
        std::set<MouseButton> mouseButtons_;
        float mouseX, mouseY;
        float deltaX, deltaY;
	};

    Key toChaiKey(int glfwKey);
}