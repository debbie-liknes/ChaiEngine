#pragma once
#include <CoreExport.h>
#include <ChaiMath.h>

namespace chai
{
    enum class CameraMode
    {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    class CORE_EXPORT ICamera 
    {
    public:
		ICamera() = default;
        virtual ~ICamera() = default;

        ICamera(const ICamera&) = delete;
        ICamera& operator=(const ICamera&) = delete;

        virtual Mat4 getProjectionMatrix() = 0;
        virtual Mat4 getViewMatrix() = 0;
        virtual void setViewMatrix(const Mat4& viewMatrix) = 0;

        virtual void setAspectRatio(float aspect) = 0;
        virtual void setFarPlane(float far) = 0;
        virtual void setNearPlan(float near) = 0;
        virtual void setFOV(float fov) = 0;
    };
}