#pragma once
#include <ChaiMathExport.h>
#include <MatBase.h>

namespace chai
{
    template <typename T>
    using Mat2T = Mat<T, 2, 2>;
    using Mat2f = Mat2T<float>;
    using Mat2d = Mat2T<double>;
    using Mat2 = Mat2f;
}