#pragma once
#include <VecBase.h>
#include <Vec3.h>
#include <Mat_4x4.h>
#include <ChaiMatrix.h>
#include <QuatBase.h>
#include <ChaiMathExport.h>

namespace chai
{
    template<typename T>
    Vec3T<T> cross(const Vec3T<T>&, const Vec3T<T>&);

    template<typename T, int N>
    Vec<T, N> normalize(const Vec<T, N>& vec);

    //template<typename T>
    //Quaternion<T> normalize(const Quaternion<T>& q);

    template<typename T>
    Mat4T<T> translate(const Mat4T<T>& m, const Vec3T<T>& offset);

    template<typename T>
    Mat4T<T> scale(const Mat4T<T>& m, const Vec3T<T>& offset);

    template<typename T>
    Mat4T<T> lookAt(const Vec3T<T>& eye, const Vec3T<T>& center, const Vec3T<T>& up);

    template<typename T, int N>
    T length(const Vec<T, N>& vec);

    template<typename T, int N>
    T dot(const Vec<T, N>& a, const Vec<T, N>& b);

    template<typename T>
    T radians(T degrees);

    template<typename T>
    Mat4T<T> perspective(T fov, T aspect, T near, T far);

    template<typename T>
    Vec<T, 3> operator*(const Quaternion<T> quat, const Vec<T, 3>& v);
}