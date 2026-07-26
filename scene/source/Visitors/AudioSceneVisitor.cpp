#include <Visitors/AudioSceneVisitor.h>

#include <Scene/GameObject.h>
#include <Components/Component.h>

#include <Components/CameraComponent.h>
#include <Components/TransformComponent.h>

namespace chai::scene
{
    void AudioSceneVisitor::visit(GameObject* node)
    {
    }

    void AudioSceneVisitor::visit(Component* node)
    {
        if (auto c = dynamic_cast<CameraComponent*>(node)) {
            auto const* t = c->getGameObject()->getComponent<TransformComponent>();
            audioData_.listeners.emplace_back(0, t->getWorldPosition(), t->forward(), t->up());
        }
    }
}
