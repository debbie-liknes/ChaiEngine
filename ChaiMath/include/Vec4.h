#pragma once
#include <VecBase.h>
#include <Vec3.h>

namespace chai
{
    template<typename T>
    class CHAIMATH_EXPORT Vec<T, 4>
    {
    private:
        std::unique_ptr<VecImpl<T, 4>> impl_;

        template<typename U, int C, int R>
        friend class MatColumnRef;

        template<typename, int>
        friend struct internal::VecAccess;

    public:
        Vec();
        Vec(const T& x, const T& y, const T& z, const T& w);
        Vec(const Vec3T<T>& v3, T w);

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

        Vec<T, 3> xyz() const;
    };

    template<typename T> using Vec4T = Vec<T, 4>;
    using Vec4f = Vec4T<float>;
    using Vec4d = Vec4T<double>;
    using Vec4 = Vec4f;
}