#pragma once
#include <MathIncludes.h>
#include <ChaiMathExport.h>

#ifndef CHAI_ROW_MAJOR
#define CHAI_ROW_MAJOR 1
#endif

namespace chai
{
    template<typename T>
    class Quaternion
    {
    public:
        using value_type = T;

        union {
            struct { 
                T x;
                T y;
                T z;
                T w; 
            };
            T data[4];
        };

        // Constructors
        Quaternion() = default;
        constexpr Quaternion(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}

        explicit constexpr Quaternion(T fill) : x(fill), y(fill), z(fill), w(fill) {}

        explicit constexpr Quaternion(std::initializer_list<T> il) 
        {
            assert(il.size() == 4 && "Quaternion init requires 4 elements");
            auto it = il.begin();
            x = *it++; y = *it++; z = *it++; w = *it++;
        }

		// Indexing
        constexpr       T& operator[](int i)       noexcept { return data[i]; }
        constexpr const T& operator[](int i) const noexcept { return data[i]; }

		// Identity quaternion
        static constexpr Quaternion identity() noexcept { return Quaternion(0, 0, 0, 1); }

        static constexpr Quaternion fromAxisAngle(const Vec3T<T>& axis, T angle_rad) noexcept;

        static constexpr Quaternion fromEulerZYX(T rollZ, T pitchX, T yawY) noexcept;

        // Basic comparisons

        constexpr bool operator==(const Quaternion& o) const noexcept 
        {
            return x == o.x && y == o.y && z == o.z && w == o.w;
        }
        constexpr bool operator!=(const Quaternion& o) const noexcept { return !(*this == o); }

		// Basic math operations

        static constexpr T dot(const Quaternion& a, const Quaternion& b) noexcept 
        {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }

        constexpr T length2() const noexcept { return dot(*this, *this); }
        T length() const noexcept { return std::sqrt(length2()); }

        Quaternion normalized() const noexcept;

        constexpr Quaternion conjugate() const noexcept { return Quaternion(-x, -y, -z, w); }
        constexpr Quaternion inverse() const noexcept;

        // Hamilton product
        friend constexpr Quaternion operator*(const Quaternion& a, const Quaternion& b) noexcept 
        {
            return Quaternion(
                a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,  // x
                a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,  // y
                a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,  // z
                a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z   // w
            );
        }

        friend constexpr Quaternion operator*(const Quaternion& q, T s) noexcept { return Quaternion(q.x * s, q.y * s, q.z * s, q.w * s); }
        friend constexpr Quaternion operator*(T s, const Quaternion& q) noexcept { return q * s; }
        friend constexpr Quaternion operator+(const Quaternion& a, const Quaternion& b) noexcept { return Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }

        // Rotate a vec3
        friend constexpr Vec3T<T> operator*(const Quaternion& q, const Vec3T<T>& v) noexcept 
        {
            T qx = q.x, qy = q.y, qz = q.z, qw = q.w;
            T tx = T(2) * (qy * v[2] - qz * v[1]);
            T ty = T(2) * (qz * v[0] - qx * v[2]);
            T tz = T(2) * (qx * v[1] - qy * v[0]);
            // v' = v + qw * t + cross(q.xyz, t)
            T vx = v[0] + qw * tx + (qy * tz - qz * ty);
            T vy = v[1] + qw * ty + (qz * tx - qx * tz);
            T vz = v[2] + qw * tz + (qx * ty - qy * tx);
            return Vec3T<T>{vx, vy, vz};
        }

        static Quaternion fromTo(const Vec3T<T>& a, const Vec3T<T>& b) noexcept;

        static Quaternion slerp(Quaternion a, Quaternion b, T t)
        {
            a = a.normalized(); b = b.normalized();
            T d = dot(a, b);
            // Take shortest path
            if (d < T(0)) { b = b * T(-1); d = -d; }

            // If very close, nlerp to avoid numerical issues
            const T DOT_THRESHOLD = T(0.9995);
            if (d > DOT_THRESHOLD)
            {
                Quaternion r = (a * (T(1) - t) + b * t).normalized();
                return r;
            }

            T theta = std::acos(d);
            T s = std::sin(theta);
            T w1 = std::sin((T(1) - t) * theta) / s;
            T w2 = std::sin(t * theta) / s;
            return (a * w1 + b * w2).normalized();
        }

        static Quaternion quatFromMat3(const Mat<T, 3, 3>& R) noexcept;

        static Quaternion quatFromMat4(const Mat<T, 4, 4>& M) noexcept;

        // Conversion to mat3/mat4
        template<int R = 3, int C = 3>
        auto toMat3() const noexcept 
        {
            static_assert(R == 3 && C == 3, "Mat3 size mismatch");
            T xx = x * x, yy = y * y, zz = z * z;
            T xy = x * y, xz = x * z, yz = y * z;
            T wx = w * x, wy = w * y, wz = w * z;

            // Row-major (default)
#if CHAI_ROW_MAJOR
            return Mat<T, 3, 3>
            {
                T(1) - T(2) * (yy + zz), T(2)* (xy - wz), T(2)* (xz + wy),
                    T(2)* (xy + wz), T(1) - T(2) * (xx + zz), T(2)* (yz - wx),
                    T(2)* (xz - wy), T(2)* (yz + wx), T(1) - T(2) * (xx + yy)
            };
#else
        // Column-major layout (values transposed in memory)
            return Mat<T, 3, 3>
            {
                T(1) - T(2) * (yy + zz), T(2)* (xy + wz), T(2)* (xz - wy),
                    T(2)* (xy - wz), T(1) - T(2) * (xx + zz), T(2)* (yz + wx),
                    T(2)* (xz + wy), T(2)* (yz - wx), T(1) - T(2) * (xx + yy)
            };
#endif
        }

        auto toMat4() const noexcept 
        {
            auto R3 = toMat3<>();
            // Promote to 4x4 with last row/col (0,0,0,1)
#if CHAI_ROW_MAJOR
            return Mat<T, 4, 4>{
                R3(0, 0), R3(0, 1), R3(0, 2), T(0),
                    R3(1, 0), R3(1, 1), R3(1, 2), T(0),
                    R3(2, 0), R3(2, 1), R3(2, 2), T(0),
                    T(0), T(0), T(0), T(1)
            };
#else
            return Mat<T, 4, 4>{
                R3(0, 0), R3(1, 0), R3(2, 0), T(0),
                    R3(0, 1), R3(1, 1), R3(2, 1), T(0),
                    R3(0, 2), R3(1, 2), R3(2, 2), T(0),
                    T(0), T(0), T(0), T(1)
            };
#endif
        }
    };

    template<typename T>
    inline T chai_qinv_sqrt(T x) { return T(1) / std::sqrt(x); }

    template<typename T>
    inline Quaternion<T> normalize(Quaternion<T> q) noexcept 
    {
        const T lsq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
        if (lsq <= T(0)) 
        {
            return Quaternion<T>(T(0), T(0), T(0), T(1));
        }
        const T inv = chai_qinv_sqrt(lsq);
        return Quaternion<T>(q.x * inv, q.y * inv, q.z * inv, q.w * inv);
    }


    using Quatf = Quaternion<float>;
    using Quatd = Quaternion<double>;
    using Quat = Quatf;
}