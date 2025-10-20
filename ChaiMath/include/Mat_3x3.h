#pragma once
#include <ChaiMathExport.h>
#include <ChaiMatrix.h>

namespace chai
{
    template<typename T> using Mat3T = Mat<T, 3, 3>;
    using Mat3f = Mat3T<float>;
    using Mat3d = Mat3T<double>;
    using Mat3 = Mat3f;
}