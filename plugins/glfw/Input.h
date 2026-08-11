/**
 * @file Input.h
 */
#pragma once
#include <Input/IInput.h>
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

        void setHoveredCamera(int32_t cameraId) override;
        int32_t getHoveredCamera() const override;
        int getActiveCamera() const override;

        const std::vector<unsigned int>& getTypedCharactersThisFrame() const override;

    private:
        std::set<Key> keys_;
        std::set<MouseButton> mouseButtons_;
        float mouseX, mouseY;
        float deltaX, deltaY;
        float scrollX, scrollY;

        std::vector<unsigned int> typedChars_;
        int32_t hoveredCamera_ = 0;
        int32_t capturedCamera_ = 0;
        bool wasDragButtonDown_ = false;
	};

    Key toChaiKey(int glfwKey);
}