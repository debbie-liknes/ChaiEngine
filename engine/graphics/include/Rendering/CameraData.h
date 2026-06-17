/**
 * @file CameraData.h
 */
#pragma once
#include <ChaiMath.h>
#include <Common/GraphicsMacros.h>

namespace chai::gfx
{
    /**
     * @brief GPU camera structure
     */
    struct alignas(16) CameraData 
    {
        math::Mat4 view;
        math::Mat4 proj;
        math::Mat4 viewProj;    //precomputed for convenience
        math::Vec3 position;    //world space eye
        float _pad0 = 0.0f;
    };

    ENFORCE_STD140_ALIGNMENT(CameraData);
}