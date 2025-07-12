#pragma once
#include <memory>
#include <initializer_list>
#include <ChaiMathExport.h>


namespace chai
{
    template<typename T>
    struct QuaternionImpl;

    template<typename T>
    class CHAIMATH_EXPORT Quaternion
    {
    private:
        std::unique_ptr<QuaternionImpl<T>> impl_;
        friend Quaternion<T> operator*<>(const Quaternion<T>& a, const Quaternion<T>& b);

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
}