#pragma once
#include <memory>
#include <initializer_list>
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
        friend Vec<T, N> operator+<>(const Vec<T, N>& a, const Vec<T, N>& b);
        friend Vec<T, N> operator-<>(const Vec<T, N>& a, const Vec<T, N>& b);
        friend Vec<T, N> operator-<>(const Vec<T, N>& v);
        friend Vec<T, N> normalize<>(const Vec<T, N>& vec);

    public:
        Vec();
        ~Vec();
        Vec(const Vec& other);
        Vec(Vec& other);
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
        template<typename U, int M>
        friend Vec<U, M> operator-(const Vec<U, M>&, const Vec<U, M>&);

    public:
        Vec();
        Vec(const T& x, const T& y);
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
        template<typename U, int M>
        friend Vec<U, M> operator-(const Vec<U, M>&, const Vec<U, M>&);

    public:
        Vec();
        Vec(const T& x, const T& y, const T& z);
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

		// Not sure how I want to handle truncation. I think id rather be explicit about it.
        Vec<T, 2> xy() const
        {
			return { x, y };
        }
    };

    template<typename T>
    class CHAIMATH_EXPORT Vec<T, 4>
    {
    private:
        std::unique_ptr<VecImpl<T, 4>> impl_;
        template<typename U, int M>
        friend Vec<U, M> operator-(const Vec<U, M>&, const Vec<U, M>&);

    public:
        Vec();
        Vec(const T& x, const T& y, const T& z, const T& w);
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

        Vec<T, 3> xyz() const 
        {
            return { x, y, z };
		}
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

    template<typename T, int N>
    Vec<T, N> operator+(const Vec<T, N>&, const Vec<T, N>&);

    template<typename T, int N>
    Vec<T, N> operator-(const Vec<T, N>&, const Vec<T, N>&);

    template<typename T, int N>
    Vec<T, N> operator-(const Vec<T, N>&);
}