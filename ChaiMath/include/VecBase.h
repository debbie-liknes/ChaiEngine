#pragma once

namespace chai
{
    template<typename Derived, typename T, size_t N>
    class VectorBase 
    {
    public:
        using ValueType = T;
        static constexpr size_t Dimensions = N;

        Derived operator+(const Derived& other) const = delete;
        Derived operator-(const Derived& other) const = delete;
        Derived operator*(T scalar) const = delete;
        Derived operator/(T scalar) const = delete;

        Derived operator-() const;

        Derived& operator+=(const Derived& other) = delete;
        Derived& operator-=(const Derived& other) = delete;
        Derived& operator*=(T scalar) = delete;
        Derived& operator/=(T scalar) = delete;

        bool operator==(const Derived& other) const = delete;
        bool operator!=(const Derived& other) const = delete;

        T dot(const Derived& other) const = delete;
        Derived normalize() const = delete;

        bool isNormalized(T epsilon = T(1e-6)) const = delete;
        bool isZero(T epsilon = T(1e-6)) const = delete;

    private:
        const Derived& derived() const 
        {
            return static_cast<const Derived&>(*this);
        }

        Derived& derived() 
        {
            return static_cast<Derived&>(*this);
        }
    };

    // Global scalar * vector operator
    template<typename Derived, typename T, size_t N>
    Derived operator*(T scalar, const VectorBase<Derived, T, N>& vec) 
    {
        return vec * scalar;
    }
}