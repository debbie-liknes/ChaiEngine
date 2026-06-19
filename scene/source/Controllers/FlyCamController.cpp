#include <Controllers/FlyCamController.h>
#include <algorithm>
#include <Components/TransformComponent.h>
#include <ChaiMath.h>

namespace chai::scene
{
    FlyCameraController::FlyCameraController(chai::scene::GameObject* obj) : IController(obj)
    {
    }

    void FlyCameraController::update(const UpdateContext& ctx)
    {
        IInput& in = ctx.input;

        // Hold right mouse to look
        const bool looking = in.mouseDown(MouseButton::Right);
        in.setCursorMode(looking ? CursorMode::Disabled : CursorMode::Normal);

        if (looking) {
            const math::Vec2 d = in.mouseDelta();
            yaw_ -= d.x * sensitivity_;
            pitch_ -= d.y * sensitivity_;
            const float limit = math::radians(89.f);
            pitch_ = std::clamp(pitch_, -limit, limit);
        }

        auto* tf = gameObject_->getComponent<TransformComponent>();
        math::Quat rot = math::Quat::fromAxisAngle({0, 1, 0}, yaw_) *
                         math::Quat::fromAxisAngle({1, 0, 0}, pitch_);
        tf->setRotation(rot);

        const math::Vec3 fwd = tf->forward();
        const math::Vec3 right = tf->right();
        const math::Vec3 up{0, 1, 0};

        // WASD + QE movement along the basis.
        math::Vec3 move{0, 0, 0};
        if (in.keyDown(Key::W))
            move = move + fwd;
        if (in.keyDown(Key::S))
            move = move - fwd;
        if (in.keyDown(Key::D))
            move = move + right;
        if (in.keyDown(Key::A))
            move = move - right;
        if (in.keyDown(Key::E))
            move = move + up;
        if (in.keyDown(Key::Q))
            move = move - up;

        const float boost = in.keyDown(Key::LeftShift) ? 4.f : 1.f;

        
        if (math::lengthSq(move) > 0.f)
            tf->setPosition(tf->getWorldPosition() + normalize(move) * speed_ * boost * ctx.dt);
    }

    void FlyCameraController::setEnabled(bool enabled)
    {
        enabled_ = enabled;
    }

    bool FlyCameraController::isEnabled() const
    {
        return enabled_;
    }

    // maybe optional
    const char* FlyCameraController::getControllerType() const
    {
        return "FlyCam";
    }
}