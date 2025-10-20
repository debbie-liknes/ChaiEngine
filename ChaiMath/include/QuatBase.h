#pragma once
#define M_PI 3.14159265358979323846
#include <cmath>

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
            struct { T x, y, z, w; };   // vector part (x,y,z), scalar part w
            T data[4];
        };

        // ---- ctors ----
        Quaternion() = default;                              // uninitialized (fast, like your Vec)
        constexpr Quaternion(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}

        // Fill ctor (splat all components)
        explicit constexpr Quaternion(T fill) : x(fill), y(fill), z(fill), w(fill) {}

        // From initializer_list<T> (strict, no narrowing)
        constexpr Quaternion(std::initializer_list<T> il) {
            assert(il.size() == 4 && "Quaternion init requires 4 elements");
            auto it = il.begin();
            x = *it++; y = *it++; z = *it++; w = *it++;
        }

        // Indexing
        constexpr       T& operator[](int i)       noexcept { return data[i]; }
        constexpr const T& operator[](int i) const noexcept { return data[i]; }

        // Identity
        static constexpr Quaternion identity() noexcept { return Quaternion(0, 0, 0, 1); }

        // From axis-angle (axis needn’t be normalized; radians)
        static constexpr Quaternion fromAxisAngle(const Vec3T<T>& axis, T angle_rad) noexcept {
            T ax = axis[0], ay = axis[1], az = axis[2];
            T len = std::sqrt(ax * ax + ay * ay + az * az);
            if (len == T(0)) return identity();
            T s = std::sin(angle_rad * T(0.5)) / len;
            T c = std::cos(angle_rad * T(0.5));
            return Quaternion(ax * s, ay * s, az * s, c);
        }

        // From Euler (YXZ is common for cameras; pick what you use. Below: ZYX (roll, pitch, yaw) order)
        // yaw (Y), pitch (X), roll (Z). Radians.
        static constexpr Quaternion fromEulerZYX(T rollZ, T pitchX, T yawY) noexcept {
            T cz = std::cos(rollZ * 0.5), sz = std::sin(rollZ * 0.5);
            T cx = std::cos(pitchX * 0.5), sx = std::sin(pitchX * 0.5);
            T cy = std::cos(yawY * 0.5), sy = std::sin(yawY * 0.5);
            // q = qy * qx * qz (ZYX intrinsic == XYZ extrinsic)
            T w_ = cy * cx * cz + sy * sx * sz;
            T x_ = cy * sx * cz + sy * cx * sz;
            T y_ = sy * cx * cz - cy * sx * sz;
            T z_ = cy * cx * sz - sy * sx * cz;
            return Quaternion(x_, y_, z_, w_);
        }

        // Basic comparisons
        constexpr bool operator==(const Quaternion& o) const noexcept {
            return x == o.x && y == o.y && z == o.z && w == o.w;
        }
        constexpr bool operator!=(const Quaternion& o) const noexcept { return !(*this == o); }

        // Dot / length / normalize
        static constexpr T dot(const Quaternion& a, const Quaternion& b) noexcept {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }
        constexpr T length2() const noexcept { return dot(*this, *this); }
        T length() const noexcept { return std::sqrt(length2()); }

        constexpr Quaternion normalized() const noexcept {
            T lsq = length2();
            if (lsq == T(0)) return identity();
            T inv = T(1) / std::sqrt(lsq);
            return Quaternion(x * inv, y * inv, z * inv, w * inv);
        }

        // Conjugate / inverse (for unit quats, inverse == conjugate)
        constexpr Quaternion conjugate() const noexcept { return Quaternion(-x, -y, -z, w); }
        constexpr Quaternion inverse() const noexcept {
            T lsq = length2();
            if (lsq == T(0)) return identity();
            T inv = T(1) / lsq;
            return Quaternion(-x * inv, -y * inv, -z * inv, w * inv);
        }

        // Hamilton product
        friend constexpr Quaternion operator*(const Quaternion& a, const Quaternion& b) noexcept {
            return Quaternion(
                a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,  // x
                a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,  // y
                a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,  // z
                a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z   // w
            );
        }

        // Scale / add (handy in integrations)
        friend constexpr Quaternion operator*(const Quaternion& q, T s) noexcept { return Quaternion(q.x * s, q.y * s, q.z * s, q.w * s); }
        friend constexpr Quaternion operator*(T s, const Quaternion& q) noexcept { return q * s; }
        friend constexpr Quaternion operator+(const Quaternion& a, const Quaternion& b) noexcept { return Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }

        // Rotate a Vec3: v' = q * v * q^{-1}  (fast form using cross products)
        friend constexpr Vec3T<T> operator*(const Quaternion& q, const Vec3T<T>& v) noexcept {
            // t = 2 * cross(q.xyz, v)
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

        // Shortest-arc quaternion from a->b (both needn’t be unit; we’ll normalize)
        static Quaternion fromTo(const Vec3T<T>& a, const Vec3T<T>& b) noexcept {
            // Normalize inputs
            auto n = [](const Vec3T<T>& v) {
                T lx = v[0], ly = v[1], lz = v[2];
                T L = std::sqrt(lx * lx + ly * ly + lz * lz);
                if (L == T(0)) return Vec3T<T>{T(0), T(0), T(0)};
                return Vec3T<T>{lx / L, ly / L, lz / L};
                };
            Vec3T<T> u = n(a), v = n(b);

            // dot and cross
            T d = u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
            Vec3T<T> c{ u[1] * v[2] - u[2] * v[1],
                        u[2] * v[0] - u[0] * v[2],
                        u[0] * v[1] - u[1] * v[0] };

            if (d < T(-1) + T(1e-6)) {
                // 180 deg: pick any orthogonal axis
                Vec3T<T> axis = std::abs(u[0]) < T(0.9) ? Vec3T<T>{T(1), 0, 0} : Vec3T<T>{ 0,T(1),0 };
                // axis = normalize(cross(u, axis))
                axis = Vec3T<T>{ u[1] * axis[2] - u[2] * axis[1],
                                 u[2] * axis[0] - u[0] * axis[2],
                                 u[0] * axis[1] - u[1] * axis[0] };
                T L = std::sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
                if (L == T(0)) axis = Vec3T<T>{ 0,0,1 };
                else { axis[0] /= L; axis[1] /= L; axis[2] /= L; }
                return fromAxisAngle(axis, T(M_PI));
            }
            // w = sqrt((1+d)*2)/2 ; xyz = c / (2w)
            T s = std::sqrt((T(1) + d) * T(2));
            T inv = T(1) / s;
            return Quaternion(c[0] * inv, c[1] * inv, c[2] * inv, s * T(0.5)).normalized();
        }

        // Spherical linear interpolation (expects unit quats; we normalize ends just in case)
        static Quaternion slerp(Quaternion a, Quaternion b, T t) noexcept {
            a = a.normalized(); b = b.normalized();
            T d = dot(a, b);
            // Take shortest path
            if (d < T(0)) { b = b * T(-1); d = -d; }

            // If very close, nlerp to avoid numerical issues
            const T DOT_THRESHOLD = T(0.9995);
            if (d > DOT_THRESHOLD) {
                Quaternion r = (a * (T(1) - t) + b * t).normalized();
                return r;
            }

            T theta = std::acos(d);
            T s = std::sin(theta);
            T w1 = std::sin((T(1) - t) * theta) / s;
            T w2 = std::sin(t * theta) / s;
            return (a * w1 + b * w2).normalized();
        }

        template<typename T>
        static inline Quaternion<T> quatFromMat3(const Mat<T, 3, 3>& R) noexcept
        {
            const T m00 = R(0, 0), m01 = R(0, 1), m02 = R(0, 2);
            const T m10 = R(1, 0), m11 = R(1, 1), m12 = R(1, 2);
            const T m20 = R(2, 0), m21 = R(2, 1), m22 = R(2, 2);

            const T trace = m00 + m11 + m22;

            Quaternion<T> q;
            if (trace > T(0)) {
                const T s = std::sqrt(trace + T(1)) * T(2); // s = 4*w
                q.w = T(0.25) * s;
                q.x = (m21 - m12) / s;
                q.y = (m02 - m20) / s;
                q.z = (m10 - m01) / s;
            }
            else if (m00 > m11 && m00 > m22) {
                const T s = std::sqrt(T(1) + m00 - m11 - m22) * T(2); // s = 4*x
                q.w = (m21 - m12) / s;
                q.x = T(0.25) * s;
                q.y = (m01 + m10) / s;
                q.z = (m02 + m20) / s;
            }
            else if (m11 > m22) {
                const T s = std::sqrt(T(1) + m11 - m00 - m22) * T(2); // s = 4*y
                q.w = (m02 - m20) / s;
                q.x = (m01 + m10) / s;
                q.y = T(0.25) * s;
                q.z = (m12 + m21) / s;
            }
            else {
                const T s = std::sqrt(T(1) + m22 - m00 - m11) * T(2); // s = 4*z
                q.w = (m10 - m01) / s;
                q.x = (m02 + m20) / s;
                q.y = (m12 + m21) / s;
                q.z = T(0.25) * s;
            }
            return q.normalized(); // defend against tiny numeric drift
        }

        template<typename T>
        static inline Quaternion<T> quatFromMat4(const Mat<T, 4, 4>& M) noexcept {
            // Take the upper-left 3x3 block as rotation
            Mat<T, 3, 3> R{
                M(0,0), M(0,1), M(0,2),
                M(1,0), M(1,1), M(1,2),
                M(2,0), M(2,1), M(2,2)
            };
            return quatFromMat3(R);
        }

        // Convert to Mat3/Mat4 (unit quaternion expected)
        template<int R = 3, int C = 3>
        auto toMat3() const noexcept {
            static_assert(R == 3 && C == 3, "Mat3 size mismatch");
            T xx = x * x, yy = y * y, zz = z * z;
            T xy = x * y, xz = x * z, yz = y * z;
            T wx = w * x, wy = w * y, wz = w * z;

            // Row-major (default)
#if CHAI_ROW_MAJOR
            return Mat<T, 3, 3>{
                T(1) - T(2) * (yy + zz), T(2)* (xy - wz), T(2)* (xz + wy),
                    T(2)* (xy + wz), T(1) - T(2) * (xx + zz), T(2)* (yz - wx),
                    T(2)* (xz - wy), T(2)* (yz + wx), T(1) - T(2) * (xx + yy)
            };
#else
        // Column-major layout (values transposed in memory)
            return Mat<T, 3, 3>{
                T(1) - T(2) * (yy + zz), T(2)* (xy + wz), T(2)* (xz - wy),
                    T(2)* (xy - wz), T(1) - T(2) * (xx + zz), T(2)* (yz + wx),
                    T(2)* (xz + wy), T(2)* (yz - wx), T(1) - T(2) * (xx + yy)
            };
#endif
        }

        auto toMat4() const noexcept {
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
    inline Quaternion<T> normalize(Quaternion<T> q) noexcept {
        const T lsq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
        if (lsq <= T(0)) {
            return Quaternion<T>(T(0), T(0), T(0), T(1));
        }
        const T inv = chai_qinv_sqrt(lsq);
        return Quaternion<T>(q.x * inv, q.y * inv, q.z * inv, q.w * inv);
    }


    // ---- Common aliases ----
    using Quatf = Quaternion<float>;
    using Quatd = Quaternion<double>;
    using Quat = Quatf;
}