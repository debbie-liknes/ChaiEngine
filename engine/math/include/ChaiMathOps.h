/**
 * @file ChaiMathOps.h
 * @brief Freestanding common math operations for Chai math types
 */

#pragma once
#include <VecBase.h>
#include <Vec3.h>
#include <Mat_4x4.h>
#include <Mat_3x3.h>
#include <ChaiQuat.h>

namespace chai::math
{
    /**
     * @brief Compute the dot product of two vectors
     */
    template <typename T, int N>
    inline T dot(const Vec<T, N>& a, const Vec<T, N>& b)
    {
        T result = T{};
        for (int i = 0; i < N; ++i)
            result += a[i] * b[i];
        return result;
    }

    /**
     * @brief Create a perspective projection matrix
     */
    template <typename T, int N>
    inline T length(const Vec<T, N>& v)
    {
        return std::sqrt(dot(v, v));
    }

    /**
     * @brief Normalize a vector
     */
    template <typename T, int N>
    inline Vec<T, N> normalize(const Vec<T, N>& v)
    {
        const T lenSq = dot(v, v);
        if (lenSq == T(0))
            return Vec<T, N>(T(0)); // explicit zero, not default-init
        return v / std::sqrt(lenSq);
    }

    /**
     * @brief Cross product of two 3D vectors
     * @param a First vector
     * @param b Second vector
     * @return The cross product of a and b
     */
    template <typename T>
    inline Vec3T<T> cross(const Vec3T<T>& a, const Vec3T<T>& b)
    {
        return Vec3T<T>{a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    }

    /**
     * @brief Convert degrees to radians
     */
    template <typename T>
    inline T radians(T degrees)
    {
        return degrees * (pi_v<T> / static_cast<T>(180));
    }

    /**
     * @brief Convert radians to degrees
     */
    template <typename T>
    inline T degrees(T radians)
    {
        return radians * (static_cast<T>(180) / pi_v<T>);
    }

    /**
     * @brief Translate a 4x4 matrix by an offset
     * @param m The matrix to translate
     * @param offset The translation offset
     * @return The translated matrix
     */
    template <typename T>
    inline Mat4T<T> translate(const Mat4T<T>& m, const Vec3T<T>& offset)
    {
        Mat4T<T> r = m; // translation lives in logical column 3
        r(0, 3) += offset.x;
        r(1, 3) += offset.y;
        r(2, 3) += offset.z;
        return r;
    }

    /**
     * @brief Scale a 4x4 matrix by an offset
     * @param m The matrix to scale
     * @param offset The scaling offset
     * @return The scaled matrix
     */
    template <typename T>
    inline Mat4T<T> scale(const Mat4T<T>& m, const Vec3T<T>& factor)
    {
        Mat4T<T> r = m;
        r(0, 0) *= factor.x;
        r(1, 1) *= factor.y;
        r(2, 2) *= factor.z;
        return r;
    }

    /**
     * @brief Create a lookAt matrix for a camera
     * @param eye The position of the camera
     * @param center The point the camera is looking at
     * @param up The up direction for the camera
     * @return The resulting lookAt matrix
     */
    template <typename T>
    inline Mat4T<T> lookAt(const Vec3T<T>& eye, const Vec3T<T>& center, const Vec3T<T>& up)
    {
        const Vec3T<T> z = normalize(eye - center); // +Z points back toward eye
        const Vec3T<T> x = normalize(cross(up, z)); // right
        const Vec3T<T> y = cross(z, x);             // true up

        Mat4T<T> r{};
        r(0, 0) = x.x;
        r(0, 1) = x.y;
        r(0, 2) = x.z;
        r(0, 3) = -dot(x, eye);
        r(1, 0) = y.x;
        r(1, 1) = y.y;
        r(1, 2) = y.z;
        r(1, 3) = -dot(y, eye);
        r(2, 0) = z.x;
        r(2, 1) = z.y;
        r(2, 2) = z.z;
        r(2, 3) = -dot(z, eye);
        r(3, 0) = 0;
        r(3, 1) = 0;
        r(3, 2) = 0;
        r(3, 3) = T(1);
        return r;
    }

    template <typename T>
    inline Mat4T<T> perspectiveGL(T fovY, T aspect, T zNear, T zFar)
    {
        assert(aspect != T(0));
        assert(zFar != zNear);
        const T f = T(1) / std::tan(fovY / T(2));
        const T inv = T(1) / (zNear - zFar);

        Mat4T<T> r{};
        r(0, 0) = f / aspect;
        r(1, 1) = f;
        r(2, 2) = (zFar + zNear) * inv;
        r(2, 3) = (T(2) * zFar * zNear) * inv;
        r(3, 2) = T(-1);
        r(3, 3) = T(0);
        return r;
    }

    template <typename T>
    inline Mat4T<T> perspectiveVK(T fovY, T aspect, T zNear, T zFar, bool flipY = true)
    {
        assert(aspect != T(0));
        assert(zFar != zNear);
        const T f = T(1) / std::tan(fovY / T(2));
        const T inv = T(1) / (zNear - zFar);

        Mat4T<T> r{};
        r(0, 0) = f / aspect;
        r(1, 1) = flipY ? -f : f;
        r(2, 2) = zFar * inv;
        r(2, 3) = (zFar * zNear) * inv;
        r(3, 2) = T(-1);
        r(3, 3) = T(0);
        return r;
    }

    /**
     * @brief Create a perspective projection matrix
     */
    template <typename T>
    inline Mat4T<T> perspective(T fovY, T aspect, T zNear, T zFar)
    {
        return perspectiveGL(fovY, aspect, zNear, zFar);
    }

    /**
     * @brief Convert a 4x4 matrix to a 3x3 matrix by dropping the last row and column
     */
    template <typename T>
    inline Mat3T<T> toMat3(const Mat4T<T>& m)
    {
        Mat3T<T> r{};
        for (int row = 0; row < 3; ++row)
            for (int col = 0; col < 3; ++col)
                r(row, col) = m(row, col);
        return r;
    }

    /**
     * @brief Convert a 3x3 matrix to a 4x4 matrix by promoting the last row and column to (0,0,0,1)
     */
    template <typename T>
    inline Mat4T<T> toMat4(const Mat3T<T>& m)
    {
        Mat4T<T> r = Mat4T<T>::identity();
        for (int row = 0; row < 3; ++row)
            for (int col = 0; col < 3; ++col)
                r(row, col) = m(row, col);
        return r;
    }
}