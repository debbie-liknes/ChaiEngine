#include <Controllers/SpinController.h>
#include <Scene/GameObject.h>
#include <ChaiMath.h>
#include <Components/TransformComponent.h>

namespace chai::scene
{
    SpinController::SpinController(chai::scene::GameObject* obj)
        : IController(obj)
    {
    }

    void SpinController::update(const UpdateContext& ctx)
    {
        auto transformComp = gameObject_->getComponent<TransformComponent>();
        float angle = math::radians(rotationSpeed_) * ctx.dt;
        math::Quat deltaRotation = math::Quat::fromAxisAngle(math::Vec3(0.0f, 1.0f, 0.0f), angle);
        math::Quat rotation = transformComp->getLocalRotation();
        rotation = deltaRotation * rotation;
        transformComp->setRotation(math::normalize(rotation));
    }

    void SpinController::setEnabled(bool enabled)
    {
        enabled_ = enabled;
    }

    bool SpinController::isEnabled() const
    {
        return enabled_;
    }
}