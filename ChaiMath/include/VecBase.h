#pragma once
#include <memory>
#include <initializer_list>
#include <ChaiMathExport.h>

namespace chai
{
    namespace internal 
    {
        template<typename T, int N>
        struct VecAccess;
    }

    template<typename T, int N>
    struct VecImpl;

    template<typename T, int C, int R>
    class MatColumnRef;

    template<typename T, int N>
    class CHAIMATH_EXPORT Vec
    {
    private:
        std::unique_ptr<VecImpl<T, N>> impl_;

        template<typename U, int C, int R>
        friend class MatColumnRef;

        template<typename, int>
        friend struct internal::VecAccess;

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

    template<typename T, int N>
    Vec<T, N> operator+(const Vec<T, N>& a, const Vec<T, N>& b);

    template<typename T, int N>
    Vec<T, N> operator-(const Vec<T, N>&, const Vec<T, N>&);

    template<typename T, int N>
    Vec<T, N> operator-(const Vec<T, N>&);
}