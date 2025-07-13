#pragma once
#include <memory>
#include <initializer_list>
#include <ChaiMathExport.h>

namespace chai
{
    template<typename T, int N>
    struct VecImpl;

    template<typename T, int N>
    class Vec;

    template<typename T, int C, int R>
    class MatColumnRef;

    template<typename T, int N>
    Vec<T, N> operator+(const Vec<T, N>& a, const Vec<T, N>& b);

    template<typename T, int N>
    Vec<T, N> operator-(const Vec<T, N>&, const Vec<T, N>&);

    template<typename T, int N>
    Vec<T, N> operator-(const Vec<T, N>&);

    template<typename T, int N>
    class CHAIMATH_EXPORT Vec
    {
    private:
        std::unique_ptr<VecImpl<T, N>> impl_;
        friend Vec<T, N> operator+<>(const Vec<T, N>& a, const Vec<T, N>& b);
        friend Vec<T, N> operator-<>(const Vec<T, N>& a, const Vec<T, N>& b);
        friend Vec<T, N> operator-<>(const Vec<T, N>& v);
        friend Vec<T, N> normalize<>(const Vec<T, N>& vec);

        template<typename U, int C, int R>
        friend class MatColumnRef;

    public:
        Vec();
        ~Vec();

        Vec(const Vec& other);
        Vec(Vec& other);
        Vec& operator=(const Vec& other);
        Vec(Vec&& other) noexcept;
        Vec& operator=(Vec&& other) noexcept;
        template<typename... Args>
        Vec(Args... args);
        Vec(std::initializer_list<T> init);
        T& operator[](int i);
        const T& operator[](int i) const;
        bool operator==(const Vec& other) const;
    };
}