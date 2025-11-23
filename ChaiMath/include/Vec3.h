#pragma once
#include <VecBase.h>

namespace chai
{
    template <typename T>
    using Vec3T = Vec<T, 3>;
    using Vec3f = Vec3T<float>;
    using Vec3d = Vec3T<double>;
    using Vec3 = Vec3f;
}