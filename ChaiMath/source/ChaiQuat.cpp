#include <ChaiMathOps.h>

namespace chai
{
    template <typename T>
    constexpr Quaternion<T> Quaternion<T>::fromAxisAngle(const Vec3T<T>& axis, T angle_rad) noexcept 
    {
        T ax = axis[0], ay = axis[1], az = axis[2];
        T len = std::sqrt(ax * ax + ay * ay + az * az);
        if (len == T(0)) return identity();
        T s = std::sin(angle_rad * T(0.5)) / len;
        T c = std::cos(angle_rad * T(0.5));
        return Quaternion(ax * s, ay * s, az * s, c);
    }

    template <typename T>
    constexpr Quaternion<T> Quaternion<T>::fromEulerZYX(T rollZ, T pitchX, T yawY) noexcept 
    {
        // static cast makes sure we dont have precision issues
        T cz = static_cast<T>(std::cos(static_cast<double>(rollZ) * 0.5));
        T sz = static_cast<T>(std::sin(static_cast<double>(rollZ) * 0.5));
        T cx = static_cast<T>(std::cos(static_cast<double>(pitchX) * 0.5));
        T sx = static_cast<T>(std::sin(static_cast<double>(pitchX) * 0.5));
        T cy = static_cast<T>(std::cos(static_cast<double>(yawY) * 0.5));
        T sy = static_cast<T>(std::sin(static_cast<double>(yawY) * 0.5));
        
        T w_ = cy * cx * cz + sy * sx * sz;
        T x_ = cy * sx * cz + sy * cx * sz;
        T y_ = sy * cx * cz - cy * sx * sz;
        T z_ = cy * cx * sz - sy * sx * cz;
        return Quaternion(x_, y_, z_, w_);
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::normalized() const noexcept
    {
        T lsq = length2();
        if (lsq == T(0)) return identity();
        T inv = T(1) / std::sqrt(lsq);
        return Quaternion(x * inv, y * inv, z * inv, w * inv);
    }

    template <typename T>
    constexpr Quaternion<T> Quaternion<T>::inverse() const noexcept 
    {
        T lsq = length2();
        if (lsq == T(0)) return identity();
        T inv = T(1) / lsq;
        return Quaternion(-x * inv, -y * inv, -z * inv, w * inv);
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::fromTo(const Vec3T<T>& a, const Vec3T<T>& b) noexcept
    {
        // normalize inputs
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

        if (d < T(-1) + T(1e-6)) 
        {
            Vec3T<T> axis = std::abs(u[0]) < T(0.9) ? Vec3T<T>{T(1), 0, 0} : Vec3T<T>{ 0,T(1),0 };
            axis = Vec3T<T>{ u[1] * axis[2] - u[2] * axis[1],
                             u[2] * axis[0] - u[0] * axis[2],
                             u[0] * axis[1] - u[1] * axis[0] };
            T L = std::sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
            if (L == T(0)) axis = Vec3T<T>{ 0,0,1 };
            else { axis[0] /= L; axis[1] /= L; axis[2] /= L; }
            return fromAxisAngle(axis, T(M_PI));
        }
        T s = std::sqrt((T(1) + d) * T(2));
        T inv = T(1) / s;
        return Quaternion(c[0] * inv, c[1] * inv, c[2] * inv, s * T(0.5)).normalized();
    }

    template<typename T>
    Quaternion<T> Quaternion<T>::quatFromMat3(const Mat<T, 3, 3>& R) noexcept
    {
        const T m00 = R(0, 0), m01 = R(0, 1), m02 = R(0, 2);
        const T m10 = R(1, 0), m11 = R(1, 1), m12 = R(1, 2);
        const T m20 = R(2, 0), m21 = R(2, 1), m22 = R(2, 2);

        const T trace = m00 + m11 + m22;

        Quaternion<T> q;
        if (trace > T(0)) 
        {
            const T s = std::sqrt(trace + T(1)) * T(2);
            q.w = T(0.25) * s;
            q.x = (m21 - m12) / s;
            q.y = (m02 - m20) / s;
            q.z = (m10 - m01) / s;
        }
        else if (m00 > m11 && m00 > m22) 
        {
            const T s = std::sqrt(T(1) + m00 - m11 - m22) * T(2);
            q.w = (m21 - m12) / s;
            q.x = T(0.25) * s;
            q.y = (m01 + m10) / s;
            q.z = (m02 + m20) / s;
        }
        else if (m11 > m22) 
        {
            const T s = std::sqrt(T(1) + m11 - m00 - m22) * T(2);
            q.w = (m02 - m20) / s;
            q.x = (m01 + m10) / s;
            q.y = T(0.25) * s;
            q.z = (m12 + m21) / s;
        }
        else 
        {
            const T s = std::sqrt(T(1) + m22 - m00 - m11) * T(2);
            q.w = (m10 - m01) / s;
            q.x = (m02 + m20) / s;
            q.y = (m12 + m21) / s;
            q.z = T(0.25) * s;
        }
        return q.normalized();
    }

    template<typename T>
    Quaternion<T> Quaternion<T>::quatFromMat4(const Mat<T, 4, 4>& M) noexcept
    {
        // take the upper-left 3x3 block as rotation
        Mat<T, 3, 3> R
        {
            M(0,0), M(0,1), M(0,2),
            M(1,0), M(1,1), M(1,2),
            M(2,0), M(2,1), M(2,2)
        };
        return quatFromMat3(R);
    }

    template class CHAIMATH_EXPORT Quaternion<float>;
    template class CHAIMATH_EXPORT Quaternion<double>;
}