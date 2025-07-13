#pragma once
#include <VecBase.h>
#include <Vec2.h>
#include <ChaiMatrix.h>

namespace chai
{
    template<typename T>
    class CHAIMATH_EXPORT Vec<T, 3>
    {
    private:
        std::unique_ptr<VecImpl<T, 3>> impl_;

        template<typename U, int C, int R>
        friend class MatColumnRef;

        template<typename, int>
        friend struct internal::VecAccess;

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
        Vec2T<T> xy() const;
    };

    template<typename T> using Vec3T = Vec<T, 3>;
    using Vec3f = Vec3T<float>;
    using Vec3d = Vec3T<double>;
    using Vec3 = Vec3f;

}