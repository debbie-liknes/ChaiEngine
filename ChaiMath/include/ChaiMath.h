#pragma once

//convenience header to include all math types
#include <Vec2.h>
#include <Vec3.h>
#include <Vec4.h>
#include <Mat_2x2.h>
#include <Mat_3x3.h>
#include <Mat_4x4.h>
#include <ChaiQuat.h>
#include <ChaiMathOps.h>

namespace chai
{
    constexpr float EPSILON_F = 1e-5f;
    constexpr double EPSILON_D = 1e-12;
}