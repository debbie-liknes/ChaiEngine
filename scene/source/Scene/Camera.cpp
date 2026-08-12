#include <Scene/Camera.h>

namespace chai::scene
{
    using namespace math;
    Mat4 Camera::getProjectionMatrix() const
    {
        //this is a perspective camera, need to have other types
        return math::perspectiveVK(fov_, aspect_, nearPlane_, farPlane_);
    }

    Mat4 Camera::getViewMatrix() const
    {
        return viewMatrix_;
    }

    void Camera::setAspectRatio(float aspect)
    {
        aspect_ = aspect;
    }

    void Camera::setFarPlane(float far)
    {
        farPlane_ = far;
    }

    void Camera::setNearPlane(float near)
    {
        nearPlane_ = near;
    }

    void Camera::setFOV(float fov)
    {
        fov_ = fov;
    }

    void Camera::setViewMatrix(const Mat4& viewMatrix)
    {
        //input comes from Transform component
        viewMatrix_ = viewMatrix;
    }
}

