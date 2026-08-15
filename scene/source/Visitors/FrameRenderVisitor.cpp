#include <Visitors/FrameRenderVisitor.h>

#include <Scene/GameObject.h>
#include <Components/Component.h>

#include <Components/CameraComponent.h>
#include <Components/LightComponent.h>
#include <Components/MeshComponent.h>
#include <Components/SkyboxComponent.h>
#include <Components/TransformComponent.h>

namespace chai::scene
{
    void FrameRenderVisitor::visit(GameObject* node)
    {
    }

    void FrameRenderVisitor::visit(Component* node)
    {
        if (auto c = dynamic_cast<CameraComponent*>(node)) {
            auto const* t = c->getGameObject()->getComponent<TransformComponent>();
            auto const& camera = c->getCamera();
            const math::Mat4 world = t ? t->getWorldMatrix() : math::Mat4::identity();
            const math::Mat4 view = world.inverse();
            renderData_.views.emplace_back(view,
                                           t->getWorldPosition(),
                                           camera.getFovY(),
                                           camera.getNearPlane(),
                                           camera.getFarPlane(),
                                           c->getGameObject()->id());
        } else if (auto c = dynamic_cast<LightComponent*>(node)) {
            // TODO: this is temporary until i get AABBs sorted
            math::Vec3 center{0.f, 0.f, 0.f};
            float radius = 10.f;

            auto const* t = c->getGameObject()->getComponent<TransformComponent>();
            auto dir = t->forward();
            const math::Mat4 world = t ? t->getWorldMatrix() : math::Mat4::identity();

            if (c->getType() == LightType::DIRECTIONAL) {
                // make a bounding sphere
                math::Vec3 dirN = math::normalize(dir);
                math::Vec3 eye = center - dirN * radius;
                math::Mat4 view = math::lookAt(eye, center, {0, 1, 0});
                math::Mat4 proj =
                    math::orthographicVK(-radius, radius, -radius, radius, 0.0f, 2.0f * radius);
                auto color = c->getColor();
                renderData_.sun = {math::Vec4{dir.x, dir.y, dir.z, 1.0},
                                   math::Vec4{color.x, color.y, color.z, c->getIntensity()},
                                   view,
                                   proj};
            }
        } else if (auto c = dynamic_cast<MeshComponent*>(node)) {
            auto const* t = c->getGameObject()->getComponent<TransformComponent>();
            const math::Mat4 model = t ? t->getWorldMatrix() : math::Mat4::identity();
            renderData_.items.emplace_back(c->getMesh(), model, c->getMaterial());
        } else if (auto c = dynamic_cast<SkyboxComponent*>(node)) {
            renderData_.environment = gfx::EnvironmentData{c->getTexture()};
        }
    }
}
