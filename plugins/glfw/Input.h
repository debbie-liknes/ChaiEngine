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
        [[nodiscard]] math::Vec2 scrollDelta() const override;

        void setCursorMode(CursorMode mode) override;

        void newFrame() override;

        void updateKeyPress(const KeyEvent& key);
        void updateMousePress(const MouseButtonEvent& mouse);
        void updateMouseMove(const MouseMoveEvent& mouse);
        void updateCharInput(const CharEvent& input);
        void updateScrollInput(const MouseScrollEvent& input);

        void consumeKeyboardEvents() override;
        void consumeMouseEvents() override;

        const std::vector<unsigned int>& getTypedCharactersThisFrame() const override;

    private:
        std::set<Key> keys_;
        std::set<MouseButton> mouseButtons_;
        float mouseX, mouseY;
        float deltaX, deltaY;
        float scrollX, scrollY;

        bool keyboardCaptured_ = false;
        bool mouseCaptured_ = false;
        std::vector<unsigned int> typedChars_;
	};

    Key toChaiKey(int glfwKey);
}