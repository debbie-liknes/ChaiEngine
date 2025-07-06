#pragma once
#include <VecBase.h>
#include <ChaiMatrix.h>
#include <ChaiQuat.h>
#include <ChaiMathExport.h>

namespace chai
{
    template<typename T, int N>
    Vec<T, N> normalize(const Vec<T, N>& vec);

    template<typename T, int N>
    T length(const Vec<T, N>& vec);

    template<typename T, int N>
    T dot(const Vec<T, N>& a, const Vec<T, N>& b);

    template<typename T>
    Vec<T, 3> cross(const Vec<T, 3>& a, const Vec<T, 3>& b);

    template<typename T>
    CHAIMATH_EXPORT Mat<T, 4> lookAt(const Vec<T, 3>& eye, const Vec<T, 3>& center, const Vec<T, 3>& up);

    template<typename T>
    CHAIMATH_EXPORT T radians(T degrees);

    template<typename T>
    CHAIMATH_EXPORT T inverse(Quaternion<T> quat);

    template<typename T, int N>
    CHAIMATH_EXPORT Mat<T, N> translate(const Mat<T, N>& mat, const Vec<T, 3>& vec);

    template<typename T, int N>
    CHAIMATH_EXPORT Mat<T, N> scale(const Mat<T, N>& mat, const Vec<T, 3>& vec);

    template<typename T>
    CHAIMATH_EXPORT Mat<T, 4> Mat4_cast(const Quaternion<T>& quat);

    template<typename T>
    CHAIMATH_EXPORT Quaternion<T> Quat_cast(const Mat<T, 4>& mat);

    template<typename T>
    CHAIMATH_EXPORT Mat<T, 4> perspective(T fov, T aspect, T near, T far);

    template<typename T>
    CHAIMATH_EXPORT Vec<T, 3> operator*(const Quaternion<T> quat, const Vec<T, 3>& v);
}