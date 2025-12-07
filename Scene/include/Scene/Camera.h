#pragma once
#include "Graphics/Frustum.h"

#include <ChaiMath.h>
#include <Scene/ICamera.h>
#include <SceneExport.h>

namespace chai::cup
{
    class SCENE_EXPORT Camera : public chai::ICamera
    {
    public:
        Camera() = default;
        ~Camera() override = default;

        Mat4 getProjectionMatrix() override;
        Mat4 getProjectionMatrix() const;
        Mat4 getViewMatrix() override;
        void setViewMatrix(const Mat4& viewMatrix) override;

        void setAspectRatio(float aspect) override;
        void setFarPlane(float far) override;
        void setNearPlan(float near) override;
        void setFOV(float fov) override;
        float getFarPlane() override;
        float getNearPlane() override;

        Frustum getFrustum() const override;

    private:
        float m_aspect = 0.0;
        float m_fov = 45.0f; // degrees
        float m_nearPlane = 0.1f;
        float m_farPlane = 1000.0f;
        Frustum m_frustum;

        Mat4 m_viewMatrix{1.f};
    };
}