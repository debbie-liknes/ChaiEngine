#pragma once
#include <VecBase.h>

namespace chai
{
    //template<typename T>
    //class CHAIMATH_EXPORT Vec<T, 4> : public VecBase<T, 4>
    //{
    //public:
    //    union
    //    {
    //        T data[4];
    //        struct { T x, y, z, w; };
    //    };

    //    //DEFINE_VEC_CONSTRUCTORS(4)
    //    //DEFINE_VEC_OPERATORS(4)

    //    //    template<class... Args>
    //    //    requires (sizeof...(Args) > 0) &&
    //    //(component_sum_v<Args...> == 4) &&
    //    //    ((is_scalar_or_vec_v<Args>) && ...) &&
    //    //    (all_convertible_to_v<T, Args...>)
    //    //    constexpr explicit Vec(const Args&... args) {
    //    //    size_t i = 0;
    //    //    append_all(i, data, static_cast<T>(args)...); // static_cast for scalar args
    //    //    // Note: append_components for Vecs already static_casts each element.
    //    //}
    //};

    template<typename T> using Vec4T = Vec<T, 4>;
    using Vec4f = Vec4T<float>;
    using Vec4d = Vec4T<double>;
    using Vec4 = Vec4f;
}