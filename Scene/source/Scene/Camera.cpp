#include <Scene/Camera.h>

namespace chai::cup
{
    Mat4 Camera::getProjectionMatrix()
    {
        //this is a perspective camera, need to have other types
        return perspective(m_fov, m_aspect, m_nearPlane, m_farPlane);
    }

    Mat4 Camera::getProjectionMatrix() const
    {
        //this is a perspective camera, need to have other types
        return perspective(m_fov, m_aspect, m_nearPlane, m_farPlane);
    }

    Mat4 Camera::getViewMatrix()
    {
        return m_viewMatrix;
    }

    void Camera::setAspectRatio(float aspect)
    {
        m_aspect = aspect;
    }

    void Camera::setFarPlane(float far)
    {
        m_farPlane = far;
    }

    void Camera::setNearPlan(float near)
    {
        m_nearPlane = near;
    }

    float Camera::getFarPlane()
    {
        return m_farPlane;
    }

    float Camera::getNearPlane()
    {
        return m_nearPlane;
    }

    void Camera::setFOV(float fov)
    {
        m_fov = fov;
    }

    void Camera::setViewMatrix(const Mat4& viewMatrix)
    {
        //input comes from Transform component
        m_viewMatrix = viewMatrix;
    }

    Frustum Camera::getFrustum() const
    {
        return Frustum::fromViewProjection(getProjectionMatrix() * m_viewMatrix);
    }
}