#include <Components/TransformComponent.h>
#include <Scene/GameObject.h>

namespace chai::scene
{
    using namespace chai::math;
    TransformComponent::TransformComponent(GameObject* owner)
        : IComponent(owner), rotation_(math::Quat::identity())
    {
    }

    Mat4 TransformComponent::getLocalMatrix() const
    {
        Mat4 t = translate(Mat4::identity(), position_);
        Mat4 r = rotation_.toMat4();
        Mat4 s = scale(Mat4::identity(), scale_);
        return t * r * s;
    }

    Mat4 TransformComponent::getWorldMatrix() const
    {
        if (auto parent = getGameObject()->getParent(); parent) {
            return parent->getComponent<TransformComponent>()->getWorldMatrix() * getLocalMatrix();
        } else {
            return getLocalMatrix();
        }
    }

    void TransformComponent::setPosition(Vec3 newPos)
    {
        position_ = newPos;
    }

    void TransformComponent::setScale(Vec3 newScale)
    {
        scale_ = newScale;
    }

    Vec3 TransformComponent::forward() const
    {
        return getWorldRotation() * Vec3{0, 0, -1};
    }

    Vec3 TransformComponent::right() const
    {
        return getWorldRotation() * Vec3{1, 0, 0};
    }

    Vec3 TransformComponent::up() const
    {
        return getWorldRotation() * Vec3{0, 1, 0};
    }

    Vec3 TransformComponent::getWorldPosition() const
    {
        Mat4 w = getWorldMatrix();
        return Vec3(w[3][0], w[3][1], w[3][2]);
    }

    Quat TransformComponent::getWorldRotation() const
    {
        if (auto parent = getGameObject()->getParent(); parent) {
            return parent->getComponent<TransformComponent>()->getWorldRotation() * rotation_;
        } else {
            return rotation_;
        }
    }

    math::Quat TransformComponent::getLocalRotation() const
    {
        return rotation_;
    }

    void TransformComponent::lookAt(const Vec3& target, const Vec3& worldUp)
    {
        Vec3 worldPos = getWorldPosition();

        Vec3 forward = normalize(target - worldPos);
        if (length(forward) < 1e-6f)
            return;

        Vec3 up = normalize(worldUp);

        // Handle the case where forward is almost parallel to up
        if (std::abs(dot(forward, up)) > 0.999f) {
            // pick a different up vector
            up = std::abs(forward.y) < 0.999f ? Vec3{0.0f, 1.0f, 0.0f} : Vec3{1.0f, 0.0f, 0.0f};
        }

        Vec3 right = normalize(cross(forward, up));
        up = cross(right, forward); // re-orthogonalize

        Mat4 rotMatrix = Mat4::identity();

        rotMatrix[0] = Vec4(right, 0.0f);
        rotMatrix[1] = Vec4(up, 0.0f);
        rotMatrix[2] = Vec4(-forward, 0.0f);
        rotMatrix[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

        Quat worldRot = Quat::quatFromMat4(rotMatrix);

        if (auto parent = getGameObject()->getParent(); parent)
            rotation_ =
                parent->getComponent<TransformComponent>()->getWorldRotation().inverse() * worldRot;
        else
            rotation_ = worldRot;
    }

} // namespace chai::cup