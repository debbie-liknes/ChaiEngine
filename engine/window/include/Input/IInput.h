#pragma once
#include <ChaiMath.h>
#include <vector>

namespace chai
{
    enum class Key {
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Space,
        LeftShift,
        LeftCtrl,
        Up,
        Down,
        Left,
        Right,
        Escape,
        Backspace,

        Invalid
    };

    enum class MouseButton { Left, Right, Middle };
    enum class CursorMode { Normal, Disabled };

    enum class InputEventType {
        KeyPress,
        KeyRelease,
        KeyRepeat,
        Char,
        MouseButtonDown,
        MouseButtonUp,
        MouseMove,
        None
    };

    /**
     * @brief Input event base class
     */
    struct InputEvent
    {
        InputEventType type = InputEventType::None;
    };

    struct KeyEvent : public InputEvent
    {
        Key key;
    };

    struct CharEvent : public InputEvent {
        unsigned int input;
    };

    struct MouseButtonEvent : public InputEvent {
        MouseButton mouse;
    };

    struct MouseMoveEvent : public InputEvent {
        float x, y;
    };

    struct MouseScrollEvent : public InputEvent {
        float x, y;
    };

    /**
     * @brief Interface to be implemented in (likely a window) plugin
     */
    class IInput
    {
    public:
        virtual ~IInput() = default;

        [[nodiscard]] virtual bool keyDown(Key k) const = 0;
        [[nodiscard]] virtual bool mouseDown(MouseButton b) const = 0;

        [[nodiscard]] virtual math::Vec2 mousePosition() const = 0;
        [[nodiscard]] virtual math::Vec2 mouseDelta() const = 0; // since last newFrame()
        [[nodiscard]] virtual math::Vec2 scrollDelta() const = 0;

        virtual void setHoveredCamera(uint64_t cameraId) = 0;
        virtual uint64_t getHoveredCamera() const = 0;
        virtual uint64_t getActiveCamera() const = 0;

        virtual void setCursorMode(CursorMode mode) = 0;

        virtual void newFrame() = 0;

        virtual const std::vector<unsigned int>& getTypedCharactersThisFrame() const = 0;
    };

    /**
     * @file SpdLogSink.h
     * @todo Feels like this belongs alongside the Updatable Interface
     */
    struct UpdateContext {
        float dt;
        IInput& input;
    };
}