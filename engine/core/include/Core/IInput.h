#pragma once
#include <ChaiMath.h>

namespace chai
{
    enum class Key {
        W,
        A,
        S,
        D,
        Q,
        E,
        Space,
        LeftShift,
        LeftCtrl,
        Up,
        Down,
        Left,
        Right,
        Escape,

        Invalid
    };

    enum class MouseButton { Left, Right, Middle };
    enum class CursorMode { Normal, Disabled };

    enum class InputEventType {
        KeyPress,
        KeyRelease,
        KeyRepeat,
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

    struct MouseButtonEvent : public InputEvent {
        MouseButton mouse;
    };

    struct MouseMoveEvent : public InputEvent {
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
        [[nodiscard]] virtual float scrollDelta() const = 0;

        virtual void setCursorMode(CursorMode mode) = 0;

        virtual void newFrame() = 0;
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