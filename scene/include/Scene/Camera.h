#pragma once
#include <SceneExport.h>
#include <ChaiMath.h>
#include <ChaiReflect.h>

namespace chai::scene
{

    class SCENE_EXPORT Camera
    {
    public:
        Camera() = default;
        ~Camera() = default;

        math::Mat4 getProjectionMatrix() const;
        math::Mat4 getViewMatrix() const;
        void setViewMatrix(const math::Mat4& viewMatrix);

        void setAspectRatio(float aspect);
        void setFarPlane(float far);
        void setNearPlane(float near);
        void setFOV(float fov);

        float getAspectRatio() const { return aspect_; }
        float getFovY() const { return fov_; }
        float getNearPlane() const { return nearPlane_; }
        float getFarPlane() const { return farPlane_; }

    private:
        friend struct ChaiReflect<Camera>;

        float aspect_ = 0.0;
        float fov_ = 45.0f; // degrees
        float nearPlane_ = 0.1f;
        float farPlane_ = 1000.0f;

        math::Mat4 viewMatrix_{1.f};
    };
}
