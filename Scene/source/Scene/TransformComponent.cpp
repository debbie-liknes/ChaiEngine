#include <Scene/TransformComponent.h>
#include <Scene/GameObject.h>

namespace chai::cup
{
	TransformComponent::TransformComponent(GameObject* owner) : Component(owner)
    {

    }

    glm::mat4 TransformComponent::getLocalMatrix() const 
    {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), m_position);
        glm::mat4 r = glm::mat4_cast(m_rotation);
        glm::mat4 s = glm::scale(glm::mat4(1.0f), m_scale);
        return t * r * s;
    }

    glm::mat4 TransformComponent::getWorldMatrix() const 
    {
		return getLocalMatrix();
    }

    void TransformComponent::setPosition(glm::vec3 newPos)
    {
        m_position = newPos;
    }

    glm::vec3 TransformComponent::forward() const 
    {
        return getWorldRotation() * glm::vec3(0, 0, -1);
    }

    glm::vec3 TransformComponent::right() const 
    {
        return getWorldRotation() * glm::vec3(1, 0, 0);
    }

    glm::vec3 TransformComponent::up() const 
    {
        return getWorldRotation() * glm::vec3(0, 1, 0);
    }

    glm::vec3 TransformComponent::getWorldPosition() const 
    {
        //if (parent) {
        //    return glm::vec3(getWorldMatrix()[3]);
        //}
        return m_position;
    }

    glm::quat TransformComponent::getWorldRotation() const 
    {
        //if (parent) {
        //    return parent->getWorldRotation() * rotation;
        //}
        return m_rotation;
    }

    void TransformComponent::lookAt(const glm::vec3& target, const glm::vec3& worldUp)
    {
        glm::vec3 worldPos = getWorldPosition();
        glm::vec3 forward = glm::normalize(target - worldPos);
        glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
        glm::vec3 up = glm::cross(right, forward);

        glm::mat3 rotMatrix;
        rotMatrix[0] = right;
        rotMatrix[1] = up;
        rotMatrix[2] = -forward;

        m_rotation = glm::quat_cast(rotMatrix);
        //if (parent) {
        //    rotation = glm::inverse(parent->getWorldRotation()) * rotation;
        //}

        //markDirty();
    }
}