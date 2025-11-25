#include <Components/TransformComponent.h>
#include <Scene/GameObject.h>

namespace chai::cup
{
    TransformComponent::TransformComponent(GameObject* owner) : Component(owner)
    {
        if (owner)
            m_parent = owner->getComponent<TransformComponent>();
    }

    Mat4 TransformComponent::getLocalMatrix() const
    {
        Mat4 t = translate(Mat4::identity(), m_position);
        Mat4 r = m_rotation.toMat4();
        Mat4 s = scale(Mat4::identity(), m_scale);
        return t * r * s;
    }

    Mat4 TransformComponent::getWorldMatrix() const
    {
        if (m_parent)
        {
            return m_parent->getWorldMatrix() * getLocalMatrix();
        }
        else
        {
            return getLocalMatrix();
        }
    }

    void TransformComponent::setPosition(chai::Vec3 newPos)
    {
        m_position = newPos;
    }

    void TransformComponent::setRotationEuler(chai::Vec3 newRot)
    {
        /*rotate()
        m_rotation = Quatf(newRot);*/
    }

    void TransformComponent::setScale(chai::Vec3 newScale)
    {
        m_scale = newScale;
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
        return m_position;
    }

    Quat TransformComponent::getWorldRotation() const
    {
        if (m_parent)
        {
            return m_parent->getWorldRotation() * m_rotation;
        }
        else
        {
            return m_rotation;
        }
    }

    void TransformComponent::lookAt(const Vec3& target, const Vec3& worldUp)
    {
        Vec3 worldPos = getWorldPosition();

        Vec3 forward = normalize(target - worldPos);
        if (length(forward) < 1e-6f)
            return;

        Vec3 up = normalize(worldUp);

        // Handle the case where forward is almost parallel to up
        if (std::abs(dot(forward, up)) > 0.999f)
        {
            // pick a different up vector
            up = std::abs(forward.y) < 0.999f ? Vec3{0.0f, 1.0f, 0.0f}
            : Vec3{1.0f, 0.0f, 0.0f};
        }

        Vec3 right = normalize(cross(forward, up));
        up = cross(right, forward); // re-orthogonalize

        Mat4 rotMatrix = Mat4::identity();

        rotMatrix[0] = Vec4(right,   0.0f);
        rotMatrix[1] = Vec4(up,      0.0f);
        rotMatrix[2] = Vec4(-forward,0.0f);
        rotMatrix[3] = Vec4(0.0f,    0.0f, 0.0f, 1.0f);

        Quat worldRot = Quat::quatFromMat4(rotMatrix);

        if (m_parent)
            m_rotation = m_parent->getWorldRotation().inverse() * worldRot;
        else
            m_rotation = worldRot;
    }


}