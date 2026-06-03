/**
 * @file Mat_2x2.h
 * @brief 2x2 matrix specialization. Provides convenient typedefs
 */
#pragma once
#include <ChaiMathExport.h>
#include <MatBase.h>

namespace chai::math
{
    template <typename T>
    using Mat2T = Mat<T, 2, 2>;
    using Mat2f = Mat2T<float>;
    using Mat2d = Mat2T<double>;
    using Mat2 = Mat2f;
}