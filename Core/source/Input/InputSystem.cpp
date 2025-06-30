#include <Input/InputSystem.h>

namespace chai
{
    InputSystem& InputSystem::instance()
    {
        static InputSystem inst;
        return inst;
    }

    void InputSystem::queueEvent(std::unique_ptr<const InputEvent> event)
    {
        eventQueue.push(std::move(event));
    }

    uint32_t InputSystem::subscribe(InputHandler handler)
    {
        uint32_t id = nextHandlerId++;
        handlers.emplace_back(id, std::move(handler));
        return id;
    }

    void InputSystem::unsubscribe(uint32_t handlerId)
    {
        handlers.erase(
            std::remove_if(handlers.begin(), handlers.end(),
                [handlerId](const auto& pair) { return pair.first == handlerId; }),
            handlers.end()
        );
    }

    void InputSystem::processEvents()
    {
        clearFrame();
        while (!eventQueue.empty())
        {
            const auto& event = eventQueue.front();

            // Update internal state for polling
            updateState(*event.get());

            // Notify subscribers
            for (const auto& [id, handler] : handlers)
            {
                handler(*event.get());
            }

            eventQueue.pop();
        }
    }

    bool InputSystem::isKeyPressed(KeyCode key) const
    {
        return keyStates[static_cast<uint32_t>(key)];
    }

    bool InputSystem::isMouseButtonPressed(MouseButton button) const
    {
        return mouseStates[static_cast<uint32_t>(button)];
    }

    void InputSystem::getMousePosition(float& x, float& y) const
    {
        x = mouseX;
        y = mouseY;
    }

    void InputSystem::getMouseDelta(float& deltaX, float& deltaY) const
    {
        deltaX = mouseDeltaX;
        deltaY = mouseDeltaY;
    }

    void InputSystem::clearFrame()
    {
        mouseDeltaX = 0.0f;
        mouseDeltaY = 0.0f;
    }

    void InputSystem::updateState(const InputEvent& event)
    {
        switch (event.type)
        {
        case InputEventType::KeyPress:
        {
            auto keyEvent = static_cast<const KeyPressEvent&>(event);
            keyStates[keyEvent.key] = true;
            break;
        }
        case InputEventType::KeyRelease:
        {
            auto keyReleaseEvent = static_cast<const KeyReleaseEvent&>(event);
            keyStates[keyReleaseEvent.key] = false;
            break;
        }
        case InputEventType::MouseButtonPress:
        {
            auto mouseEvent = static_cast<const MouseDownEvent&>(event);
            mouseStates[mouseEvent.button] = true;
            break;
        }
        case InputEventType::MouseButtonRelease:
        {
            auto mouseReleaseEvent = static_cast<const MouseUpEvent&>(event);
            mouseStates[mouseReleaseEvent.button] = false;
            break;
        }
        case InputEventType::MouseMove:
        {
            auto mouseMoveEvent = static_cast<const MouseMoveEvent&>(event);
            mouseDeltaX = mouseMoveEvent.xPos - mouseX;
            mouseDeltaY = mouseMoveEvent.yPos - mouseY;
            mouseX = mouseMoveEvent.xPos;
            mouseY = mouseMoveEvent.yPos;
            break;
        }
        default:
            break;
        }
    }
}