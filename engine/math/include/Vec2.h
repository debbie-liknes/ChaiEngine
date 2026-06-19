/**
 * @file Vec2.h
 * @brief A simple 2D vector specialization of the Vec class template
 */
#pragma once
#include <VecBase.h>

namespace chai::math
{
    template <typename T>
    using Vec2T = Vec<T, 2>;
    using Vec2f = Vec2T<float>;
    using Vec2d = Vec2T<double>;
    using Vec2 = Vec2f;
}