#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

#include <Vec2.h>
#include <Vec3.h>
#include <Vec4.h>
#include <Mat_2x2.h>
#include <Mat_3x3.h>
#include <Mat_4x4.h>
#include <QuatBase.h>
#include <ChaiMathOps.h>

namespace chai
{
	constexpr float  EPSILON_F = 1e-5f;
	constexpr double EPSILON_D = 1e-12;
}