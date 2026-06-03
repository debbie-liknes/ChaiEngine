/**
 * @file Mat_4x4.h
 * @brief 4x4 matrix specialization. Provides convenient typedefs
 */
#pragma once
#include <MatBase.h>

namespace chai::math
{
    template <typename T>
    using Mat4T = Mat<T, 4, 4>;
    using Mat4f = Mat4T<float>;
    using Mat4d = Mat4T<double>;

    using Mat4 = Mat4f;
}