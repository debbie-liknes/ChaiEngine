#pragma once
#include <memory>
#include <initializer_list>
#include <VecTraits.h>
#include <ChaiMathExport.h>

namespace chai
{
    template<typename T, int N>
    struct VecImpl;

    template<typename T, int N>
    class CHAIMATH_EXPORT Vec
    {
    private:
        std::unique_ptr<VecImpl<T, N>> impl_;
        friend Vec<T, N> operator-<>(const Vec<T, N>& a, const Vec<T, N>& b);
    public:
        Vec();
        ~Vec();
        Vec(const Vec& other);
        Vec& operator=(const Vec& other);
        Vec(Vec&& other) noexcept;
        Vec& operator=(Vec&& other) noexcept;
        template<typename... Args>
        explicit Vec(Args... args);
        Vec(std::initializer_list<T> init);
        T& operator[](int i);
        const T& operator[](int i) const;
        bool operator==(const Vec& other) const;
    };

    template<typename T>
    class CHAIMATH_EXPORT Vec<T, 2>
    {
    private:
        std::unique_ptr<VecImpl<T, 2>> impl_;
        friend Vec<T, 2> operator-<>(const Vec<T, 2>& a, const Vec<T, 2>& b);

    public:
        Vec();
        ~Vec();
        Vec(const Vec& other);
        Vec& operator=(const Vec& other);
        Vec(Vec&& other) noexcept;
        Vec& operator=(Vec&& other) noexcept;
        template<typename... Args>
        explicit Vec(Args... args);
        Vec(std::initializer_list<T> init);
        T& operator[](int i);
        const T& operator[](int i) const;
        bool operator==(const Vec& other) const;

        T& x;
        T& y;
    };

    template<typename T>
    class CHAIMATH_EXPORT Vec<T, 3>
    {
    private:
        std::unique_ptr<VecImpl<T, 3>> impl_;
        friend Vec<T, 3> operator-<>(const Vec<T, 3>& a, const Vec<T, 3>& b);
    public:
        Vec();
        ~Vec();
        Vec(const Vec& other);
        Vec& operator=(const Vec& other);
        Vec(Vec&& other) noexcept;
        Vec& operator=(Vec&& other) noexcept;
        template<typename... Args>
        explicit Vec(Args... args);
        Vec(std::initializer_list<T> init);
        T& operator[](int i);
        const T& operator[](int i) const;
        bool operator==(const Vec& other) const;

        T& x;
        T& y;
		T& z;
    };

    // Specialization for Vec4 - x(), y(), z(), and w()
    template<typename T>
    class CHAIMATH_EXPORT Vec<T, 4>
    {
    private:
        std::unique_ptr<VecImpl<T, 4>> impl_;
        friend Vec<T, 4> operator-<>(const Vec<T, 4>& a, const Vec<T, 4>& b);
    public:
        Vec();
        ~Vec();
        Vec(const Vec& other);
        Vec& operator=(const Vec& other);
        Vec(Vec&& other) noexcept;
        Vec& operator=(Vec&& other) noexcept;
        template<typename... Args>
        explicit Vec(Args... args);
        Vec(std::initializer_list<T> init);
        T& operator[](int i);
        const T& operator[](int i) const;
        bool operator==(const Vec& other) const;

		T& x;
		T& y;
        T& z;
        T& w;
    };

    template<typename T> using Vec2T = Vec<T, 2>;
    template<typename T> using Vec3T = Vec<T, 3>;
    template<typename T> using Vec4T = Vec<T, 4>;
    using Vec2f = Vec2T<float>;
    using Vec3f = Vec3T<float>;
    using Vec4f = Vec4T<float>;
    using Vec2d = Vec2T<double>;
    using Vec3d = Vec3T<double>;
    using Vec4d = Vec4T<double>;
    using Vec2 = Vec2f;
    using Vec3 = Vec3f;
    using Vec4 = Vec4f;

    // Arithmetic operations

    template<typename T, int N>
    Vec<T, N> operator-(const Vec<T, N>& a, const Vec<T, N>& b);

    template<typename T, int N>
    Vec<T, N> operator-(const Vec<T, N>& v);
}