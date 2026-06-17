/**
 * @file ChaiQuat.h
 * @brief A simple quaternion class for representing 3D rotations
 */
#pragma once

#include <MatBase.h>
#include <MathIncludes.h>
#include <Vec3.h>
#include <VecBase.h>
#include <cmath>

namespace chai::math
{
    /**
     * @brief A simple quaternion class for representing 3D rotations. Stores components as (x, y,
     * z, w) where w is the scalar part.
     */
    template <typename T>
    class Quaternion
    {
    public:
        using value_type = T;
        union {
            struct {
                T x, y, z, w;
            };
            T data[4];
        };

        Quaternion() = default;
        constexpr Quaternion(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
        explicit constexpr Quaternion(T fill) : x(fill), y(fill), z(fill), w(fill) {}
        explicit constexpr Quaternion(std::initializer_list<T> il)
        {
            assert(il.size() == 4 && "Quaternion init requires 4 elements");
            auto it = il.begin();
            x = *it++;
            y = *it++;
            z = *it++;
            w = *it++;
        }

        constexpr T& operator[](int i) noexcept { return data[i]; }
        constexpr const T& operator[](int i) const noexcept { return data[i]; }

        /**
         * @return The imaginary part of the quaternion
         */
        constexpr Vec3T<T> vec() const noexcept { return Vec3T<T>{x, y, z}; }

        static constexpr Quaternion identity() noexcept { return Quaternion(0, 0, 0, 1); }

        constexpr bool operator==(const Quaternion& o) const noexcept
        {
            return x == o.x && y == o.y && z == o.z && w == o.w;
        }
        constexpr bool operator!=(const Quaternion& o) const noexcept { return !(*this == o); }

        /**
         * @return Dot product between 2 quaternions
         */
        static constexpr T dot(const Quaternion& a, const Quaternion& b) noexcept
        {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }

        /**
         * @return Length squared
         */
        constexpr T length2() const noexcept { return dot(*this, *this); }

        /**
         * @return Length
         */
        T length() const noexcept { return std::sqrt(length2()); }

        /**
         * @brief Reverses the direction of the Quaternion
         * @return Conjugated quaternion
         */
        constexpr Quaternion conjugate() const noexcept { return Quaternion(-x, -y, -z, w); }

        /**
         * @brief Similar to conjugate, but it scales it
         * @return Inverted quaternion
         */
        constexpr Quaternion inverse() const noexcept
        {
            const T lsq = length2();
            if (lsq == T(0))
                return identity();
            const T inv = T(1) / lsq;
            return Quaternion(-x * inv, -y * inv, -z * inv, w * inv);
        }

        /**
         * @return Normalized Quaternion
         */
        Quaternion normalized() const noexcept
        {
            const T lsq = length2();
            if (lsq == T(0))
                return identity();
            const T inv = T(1) / std::sqrt(lsq);
            return Quaternion(x * inv, y * inv, z * inv, w * inv);
        }

        // ---- operators -------------------------------------------------------
        friend constexpr Quaternion operator*(const Quaternion& a, const Quaternion& b) noexcept
        {
            return Quaternion(a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
                              a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
                              a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
                              a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);
        }
        friend constexpr Quaternion operator*(const Quaternion& q, T s) noexcept
        {
            return Quaternion(q.x * s, q.y * s, q.z * s, q.w * s);
        }
        friend constexpr Quaternion operator*(T s, const Quaternion& q) noexcept { return q * s; }
        friend constexpr Quaternion operator+(const Quaternion& a, const Quaternion& b) noexcept
        {
            return Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
        }

        /**
         * @brief A more efficient way to rotate a vector v by a quaternion q
         * @note This is equivalent to q * Quaternion(v.x, v.y, v.z, 0) * q.inverse(), but avoids
         * the intermediate quaternion construction and multiplication
         */
        friend constexpr Vec3T<T> operator*(const Quaternion& q, const Vec3T<T>& v) noexcept
        {
            const T qx = q.x, qy = q.y, qz = q.z, qw = q.w;
            // t = 2 * (q x v)
            const T tx = T(2) * (qy * v[2] - qz * v[1]);
            const T ty = T(2) * (qz * v[0] - qx * v[2]);
            const T tz = T(2) * (qx * v[1] - qy * v[0]);
            return Vec3T<T>{v[0] + qw * tx + (qy * tz - qz * ty),
                            v[1] + qw * ty + (qz * tx - qx * tz),
                            v[2] + qw * tz + (qx * ty - qy * tx)};
        }

        /**
         * @brief Create a quaternion representing a rotation of angle_rad radians around the axis
         */
        static Quaternion fromAxisAngle(const Vec3T<T>& axis, T angle_rad) noexcept
        {
            const T ax = axis[0], ay = axis[1], az = axis[2];
            const T len = std::sqrt(ax * ax + ay * ay + az * az);
            if (len == T(0))
                return identity();
            const T s = std::sin(angle_rad * T(0.5)) / len;
            const T c = std::cos(angle_rad * T(0.5));
            return Quaternion(ax * s, ay * s, az * s, c);
        }

        /**
         * @brief Create a quaternion from Euler angles
         * 
         * Equivalent quaternion composition:
         * q = qYawY * qPitchX * qRollZ
         * 
         * @note Angles are expected in radians
         */
        static Quaternion fromEuler(T zRotation, T xRotation, T yRotation) noexcept
        {
            const T cz = static_cast<T>(std::cos(static_cast<double>(zRotation) * 0.5));
            const T sz = static_cast<T>(std::sin(static_cast<double>(zRotation) * 0.5));
            const T cx = static_cast<T>(std::cos(static_cast<double>(xRotation) * 0.5));
            const T sx = static_cast<T>(std::sin(static_cast<double>(xRotation) * 0.5));
            const T cy = static_cast<T>(std::cos(static_cast<double>(yRotation) * 0.5));
            const T sy = static_cast<T>(std::sin(static_cast<double>(yRotation) * 0.5));
            return Quaternion(cy * sx * cz + sy * cx * sz,
                              sy * cx * cz - cy * sx * sz,
                              cy * cx * sz - sy * sx * cz,
                              cy * cx * cz + sy * sx * sz);
        }

        /**
         * @brief Construct the shortest arc rotation that maps vector a onto vector b.
         * For non-opposite vectors:
         * axis = cross(a, b)
         * angle = acos(dot(a, b))
         * 
         * Assumes unit vectors.
         * 
         * @return A normalized quaternion
         */
        static Quaternion fromTo(const Vec3T<T>& a, const Vec3T<T>& b) noexcept
        {
            auto norm = [](const Vec3T<T>& v) -> Vec3T<T> {
                const T L = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
                if (L == T(0))
                    return Vec3T<T>{T(0), T(0), T(0)};
                return Vec3T<T>{v[0] / L, v[1] / L, v[2] / L};
            };
            const Vec3T<T> u = norm(a), v = norm(b);
            const T d = u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
            Vec3T<T> c{
                u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2], u[0] * v[1] - u[1] * v[0]};

            if (d < T(-1) + T(1e-6))
            {
                Vec3T<T> axis =
                    std::fabs(u[0]) < T(0.9) ? Vec3T<T>{T(1), 0, 0} : Vec3T<T>{0, T(1), 0};
                axis = Vec3T<T>{u[1] * axis[2] - u[2] * axis[1],
                                u[2] * axis[0] - u[0] * axis[2],
                                u[0] * axis[1] - u[1] * axis[0]};
                const T L = std::sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
                if (L == T(0))
                    axis = Vec3T<T>{0, 0, 1};
                else {
                    axis[0] /= L;
                    axis[1] /= L;
                    axis[2] /= L;
                }
                return fromAxisAngle(axis, pi_v<T>);
            }
            const T s = std::sqrt((T(1) + d) * T(2));
            const T inv = T(1) / s;
            return Quaternion(c[0] * inv, c[1] * inv, c[2] * inv, s * T(0.5)).normalized();
        }

         /**
         * @brief Spherical linear interpolation between two quaternions.
         * 
         * When the quaternions are nearly identical, falls back to normalized
         * linear interpolation (NLERP) to avoid numerical instability caused by
         * sin(theta) approaching zero.
         */
        static Quaternion slerp(Quaternion a, Quaternion b, T t)
        {
            a = a.normalized();
            b = b.normalized();
            T d = dot(a, b);
            if (d < T(0)) {
                // q and -q represent the same orientation.
                // Flip one quaternion so interpolation follows the
                //shortest arc on the 4D unit sphere.
                b = b * T(-1);
                d = -d;
            }
            constexpr T kThreshold = T(0.9995);
            if (d > kThreshold)
                return (a * (T(1) - t) + b * t).normalized();
            const T theta = std::acos(d);
            const T s = std::sin(theta);
            const T w1 = std::sin((T(1) - t) * theta) / s;
            const T w2 = std::sin(t * theta) / s;
            return (a * w1 + b * w2).normalized();
        }

        /**
         * @brief Spherical linear interpolation between two quaternions.
         *
         * When the quaternions are nearly identical, falls back to normalized
         * linear interpolation (NLERP) to avoid numerical instability caused by
         * sin(theta) approaching zero.
         */
        Mat<T, 3, 3> toMat3() const noexcept
        {
            const T xx = x * x, yy = y * y, zz = z * z, xy = x * y, xz = x * z, yz = y * z,
                    wx = w * x, wy = w * y, wz = w * z;
            Mat<T, 3, 3> R{};
            R(0, 0) = T(1) - T(2) * (yy + zz);
            R(0, 1) = T(2) * (xy - wz);
            R(0, 2) = T(2) * (xz + wy);
            R(1, 0) = T(2) * (xy + wz);
            R(1, 1) = T(1) - T(2) * (xx + zz);
            R(1, 2) = T(2) * (yz - wx);
            R(2, 0) = T(2) * (xz - wy);
            R(2, 1) = T(2) * (yz + wx);
            R(2, 2) = T(1) - T(2) * (xx + yy);
            return R;
        }

        /**
         * @brief Convert the quaternion to a 4x4 rotation matrix
         * @note The resulting matrix will have the upper-left 3x3 portion as the rotation, and the
         * rest will be identity (no translation or scaling)
         */
        Mat<T, 4, 4> toMat4() const noexcept
        {
            const Mat<T, 3, 3> R = toMat3();
            Mat<T, 4, 4> M = Mat<T, 4, 4>::identity();
            for (int r = 0; r < 3; ++r)
                for (int c = 0; c < 3; ++c)
                    M(r, c) = R(r, c);
            return M;
        }

        /**
         * @brief Create a quaternion from a 3x3 rotation matrix.
         * Assumes the matrix is a pure rotation
         * 
         * @return A normalized quaternion
         */
        static Quaternion quatFromMat3(const Mat<T, 3, 3>& R) noexcept
        {
            const T m00 = R(0, 0), m01 = R(0, 1), m02 = R(0, 2);
            const T m10 = R(1, 0), m11 = R(1, 1), m12 = R(1, 2);
            const T m20 = R(2, 0), m21 = R(2, 1), m22 = R(2, 2);
            const T trace = m00 + m11 + m22;
            Quaternion q;
            if (trace > T(0)) {
                const T s = std::sqrt(trace + T(1)) * T(2);
                q.w = T(0.25) * s;
                q.x = (m21 - m12) / s;
                q.y = (m02 - m20) / s;
                q.z = (m10 - m01) / s;
            } else if (m00 > m11 && m00 > m22) {
                const T s = std::sqrt(T(1) + m00 - m11 - m22) * T(2);
                q.w = (m21 - m12) / s;
                q.x = T(0.25) * s;
                q.y = (m01 + m10) / s;
                q.z = (m02 + m20) / s;
            } else if (m11 > m22) {
                const T s = std::sqrt(T(1) + m11 - m00 - m22) * T(2);
                q.w = (m02 - m20) / s;
                q.x = (m01 + m10) / s;
                q.y = T(0.25) * s;
                q.z = (m12 + m21) / s;
            } else {
                const T s = std::sqrt(T(1) + m22 - m00 - m11) * T(2);
                q.w = (m10 - m01) / s;
                q.x = (m02 + m20) / s;
                q.y = (m12 + m21) / s;
                q.z = T(0.25) * s;
            }
            return q.normalized();
        }

        /**
         * @brief Create a quaternion from a 4x4 rotation matrix. Assumes the upper-left 3x3 portion
         * is a pure rotation and the rest of the matrix is identity
         *
         * @return A normalized quaternion
         */
        static Quaternion quatFromMat4(const Mat<T, 4, 4>& M) noexcept
        {
            Mat<T, 3, 3> R{};
            for (int r = 0; r < 3; ++r)
                for (int c = 0; c < 3; ++c)
                    R(r, c) = M(r, c);
            return quatFromMat3(R);
        }
    };

    /**
     * @brief Free function to normalize a quaternion
     *
     * @return A normalized quaternion
     */
    template <typename T>
    inline Quaternion<T> normalize(const Quaternion<T>& q) noexcept
    {
        return q.normalized();
    }

    using Quatf = Quaternion<float>;
    using Quatd = Quaternion<double>;
    using Quat = Quatf;
} // namespace chai