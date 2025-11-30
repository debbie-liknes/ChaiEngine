#include <Components/TransformComponent.h>
#include <Scene/GameObject.h>

namespace chai::cup
{
    TransformComponent::TransformComponent(GameObject* owner) : Component(owner)
    {
    }

    Mat4 TransformComponent::getLocalMatrix() const
    {
        Mat4 t = translate(Mat4::identity(), m_position);
        Mat4 r = m_rotation.toMat4();
        Mat4 s = scale(Mat4::identity(), m_scale);
        return t * r * s;
    }

    void TransformComponent::setLocalMatrix(const Mat4& matrix)
    {
        m_position = Vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
        m_rotation = Quat::quatFromMat4(matrix);
        m_scale = Vec3(length(Vec3(matrix[0][0], matrix[0][1], matrix[0][2])),
                       length(Vec3(matrix[1][0], matrix[1][1], matrix[1][2])),
                       length(Vec3(matrix[2][0], matrix[2][1], matrix[2][2])));
    }

    Mat4 TransformComponent::getWorldMatrix() const
    {
        if (auto parent = getGameObject()->getParent(); parent) {
            return parent->getComponent<TransformComponent>()->getWorldMatrix() * getLocalMatrix();
        } else {
            return getLocalMatrix();
        }
    }

    void TransformComponent::setPosition(chai::Vec3 newPos)
    {
        m_position = newPos;
    }

    void TransformComponent::setRotationEuler(chai::Vec3 newRot)
    {
        //auto y = m_rotation * newRot;
        //rotate()
        //m_rotation = Quatf(newRot);
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
        if (auto parent = getGameObject()->getParent(); parent) {
            return parent->getComponent<TransformComponent>()->getWorldRotation() * m_rotation;
        } else {
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
            m_rotation =
                parent->getComponent<TransformComponent>()->getWorldRotation().inverse() * worldRot;
        else
            m_rotation = worldRot;
    }

} // namespace chai::cup