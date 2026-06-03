/**
 * @file ChaiMath.h
 * @brief Convenience header to include all math types
 */
#pragma once

#include <Vec2.h>
#include <Vec3.h>
#include <Vec4.h>
#include <Mat_2x2.h>
#include <Mat_3x3.h>
#include <Mat_4x4.h>
#include <ChaiQuat.h>
#include <ChaiMathOps.h>

namespace chai::math
{
    constexpr float kEpsilonF = 1e-5f;
    constexpr double kEpsilonD = 1e-12;
}