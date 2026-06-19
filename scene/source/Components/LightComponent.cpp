#include <Components/LightComponent.h>
#include <Components/TransformComponent.h>
#include <Rendering/FrameRenderData.h>
#include <Scene/GameObject.h>

namespace chai::scene
{
    LightComponent::LightComponent(GameObject* owner) : Component(owner) {}

    void LightComponent::extract(gfx::FrameRenderData& frame) const
    {
        // TODO: this is temporary until i get AABBs sorted
        math::Vec3 center{0.f, 0.f, 0.f};
        float radius = 10.f;

        auto const* t = getGameObject()->getComponent<TransformComponent>();
        auto dir = t->forward();
        const math::Mat4 world = t ? t->getWorldMatrix() : math::Mat4::identity();

        if (type_ == LightType::DIRECTIONAL) {
            //make a bounding sphere
            math::Vec3 dirN = math::normalize(dir);
            math::Vec3 eye = center - dirN * radius;
            math::Mat4 view = math::lookAt(eye, center, {0, 1, 0});
            math::Mat4 proj =
                math::orthographicVK(-radius, radius, -radius, radius, 0.0f, 2.0f * radius);
            frame.sun = {
                math::Vec4{dir.x, dir.y, dir.z, 1.0},
                math::Vec4{color_.x, color_.y, color_.z, intensity_},
                view, proj};
        }
    }
} // namespace chai::scene