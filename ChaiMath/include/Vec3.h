#pragma once
#include <VecBase.h>

namespace chai
{
    //template<typename T>
    //class CHAIMATH_EXPORT Vec<T, 3> : public VecBase<T, 3>
    //{
    //public:
    //    union
    //    {
    //        T data[3];
    //        struct { T x, y, z; };
    //    };

    //    //DEFINE_VEC_CONSTRUCTORS(3)
    //    //DEFINE_VEC_OPERATORS(3)
    //};

    template<typename T> using Vec3T = Vec<T, 3>;
    using Vec3f = Vec3T<float>;
    using Vec3d = Vec3T<double>;
    using Vec3 = Vec3f;

}