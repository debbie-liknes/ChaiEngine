/**
 * @file Mat_3x3.h
 * @brief 3x3 matrix specialization. Provides convenient typedefs
 */
#pragma once
#include <MatBase.h>

namespace chai::math
{
    template <typename T>
    using Mat3T = Mat<T, 3, 3>;
    using Mat3f = Mat3T<float>;
    using Mat3d = Mat3T<double>;
    using Mat3 = Mat3f;
}