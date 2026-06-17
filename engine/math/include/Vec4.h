/**
 * @file Vec4.h
 * @brief A simple 4D vector specialization of the Vec class template
 */
#pragma once
#include <VecBase.h>

namespace chai::math
{
    template <typename T>
    using Vec4T = Vec<T, 4>;
    using Vec4f = Vec4T<float>;
    using Vec4d = Vec4T<double>;
    using Vec4 = Vec4f;
}