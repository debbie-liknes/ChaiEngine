#pragma once
#include <VecBase.h>

namespace chai
{
    template<typename T>
    class CHAIMATH_EXPORT Vec<T, 2>
    {
    private:
        std::unique_ptr<VecImpl<T, 2>> impl_;

        template<typename U, int M>
        friend Vec<U, M> operator-(const Vec<U, M>&, const Vec<U, M>&);

        template<typename U, int M>
        friend Vec<U, M> operator+(const Vec<U, M>&, const Vec<U, M>&);

        template<typename U, int M>
        friend Vec<U, M> operator-(const Vec<U, M>&);

        template<typename U, int C, int R>
        friend class MatColumnRef;

    public:
        Vec();
        Vec(const T& x, const T& y);
        ~Vec();
        Vec(const Vec& other);
        Vec& operator=(const Vec& other);
        Vec(Vec&& other) noexcept;
        Vec& operator=(Vec&& other) noexcept;
        template<typename... Args>
        Vec(Args... args);
        Vec(std::initializer_list<T> init);
        T& operator[](int i);
        const T& operator[](int i) const;
        bool operator==(const Vec& other) const;

        T& x;
        T& y;
    };

    template<typename T> using Vec2T = Vec<T, 2>;
    using Vec2f = Vec2T<float>;
    using Vec2d = Vec2T<double>;
    using Vec2 = Vec2f;

}