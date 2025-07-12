#include <Components/TransformComponent.h>
#include <Scene/GameObject.h>

namespace chai::cup
{
	TransformComponent::TransformComponent(GameObject* owner) : Component(owner)
    {
        if(owner)
		    m_parent = owner->getComponent<TransformComponent>();
    }

    Mat4 TransformComponent::getLocalMatrix() const 
    {
        Mat4 t = translate(Mat4{ 1.0f }, m_position);
        Mat4 r = Mat4_cast(m_rotation);
        Mat4 s = scale(Mat4{ 1.0f }, m_scale);
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

    Vec3 TransformComponent::forward() const 
    {
        return getWorldRotation() * Vec3{ 0, 0, -1 };
    }

    Vec3 TransformComponent::right() const 
    {
        return getWorldRotation() * Vec3{ 1, 0, 0 };
    }

    Vec3 TransformComponent::up() const 
    {
        return getWorldRotation() * Vec3{ 0, 1, 0 };
    }

    Vec3 TransformComponent::getWorldPosition() const 
    {
        return Vec3(getWorldMatrix()[3].xyz());
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
        Vec3 right = normalize(cross(forward, worldUp));
        Vec3 up = cross(right, forward);

        Mat4 rotMatrix;
        //rotMatrix[0] = { right, 1.0 };
        //rotMatrix[1] = { up, 1.0 };
        //rotMatrix[2] = { -forward, 1.0 };

        m_rotation = Quat_cast(rotMatrix);

        if (m_parent) 
        {
            // Convert world rotation to local
            m_rotation = inverse(m_parent->getWorldRotation()) * m_rotation;
        }
    }
}