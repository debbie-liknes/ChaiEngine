#pragma once
#include <VecBase.h>

namespace chai
{
    template <typename T>
    using Vec2T = Vec<T, 2>;
    using Vec2f = Vec2T<float>;
    using Vec2d = Vec2T<double>;
    using Vec2 = Vec2f;
}