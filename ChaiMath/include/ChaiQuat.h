#pragma once
#include <memory>
#include <initializer_list>
#include <ChaiMathExport.h>

namespace chai
{
    namespace internal
    {
        template<typename T>
        struct QuatAccess;
    }

    template<typename T>
    struct QuaternionImpl;

    template<typename T>
    class Quaternion;

    template<typename T>
    Quaternion<T> inverse(Quaternion<T> quat);

    template<typename T>
    class CHAIMATH_EXPORT Quaternion
    {
    private:
        std::unique_ptr<QuaternionImpl<T>> impl_;
        friend Quaternion<T> operator*<>(const Quaternion<T>& a, const Quaternion<T>& b);
        friend Quaternion<T> inverse<T>(Quaternion<T> quat);

        template<typename T>
        friend struct internal::QuatAccess;

    public:
        Quaternion();
        ~Quaternion();

        // Copy constructor and assignment
        Quaternion(const Quaternion& other);
        Quaternion& operator=(const Quaternion& other);

        // Move constructor and assignment
        Quaternion(Quaternion&& other) noexcept;
        Quaternion& operator=(Quaternion&& other) noexcept;

        // Constructor from axis-angle (angle in radians, axis as normalized vector)
        Quaternion(T angle, T x, T y, T z);

        // Constructor from Euler angles (pitch, yaw, roll in radians)
        Quaternion(T pitch, T yaw, T roll);

        // Component access
        T& operator[](int i);
        const T& operator[](int i) const;

        // Named component access
        T& w();
        const T& w() const;
        T& x();
        const T& x() const;
        T& y();
        const T& y() const;
        T& z();
        const T& z() const;

        bool operator==(const Quaternion& other) const;
        bool operator!=(const Quaternion& other) const;
    };

    // Type aliases
    using Quatf = Quaternion<float>;
    using Quatd = Quaternion<double>;
	using Quat = Quatf;

    template<typename T>
    Quaternion<T> operator*(const Quaternion<T>& a, const Quaternion<T>& b);

    //this is dubious
    template<typename T>
    Vec<T, 4> operator*(const Quaternion<T>& a, const Vec<T, 4>& b);

    //dubious pt 2
    template<typename T>
    Vec<T, 4> operator*(const Vec<T, 4>& a, const Quaternion<T>& b);
}