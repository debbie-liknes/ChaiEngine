#pragma once
#include <ChaiMathExport.h>
#include <MatBase.h>

namespace chai
{
    template<typename T> using Mat4T = Mat<T, 4, 4>;
    using Mat4f = Mat4T<float>;
    using Mat4d = Mat4T<double>;
    using Mat4 = Mat4f;
}