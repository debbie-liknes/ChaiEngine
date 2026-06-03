/**
 * @file VecBase.h
 * @brief A simple vector class template with basic operations and flexible construction.
 */
#pragma once
#include <MathIncludes.h>
#include <cassert>
#include <initializer_list>
#include <memory>
#include <type_traits>

namespace chai::math
{
    /**
     * @brief A simple vector class template.
     */
    template <typename T, int N>
    class Vec;

    /**
     * @brief Type trait to detect if a type is a Vec, and to extract the scalar type and component
     * count from a Vec.
     */
    template <class>
    struct is_vec : std::false_type {
    };

    /**
     * @brief Specialization of is_vec for Vec types, which inherits from std::true_type to indicate
     * that the type is a Vec
     */
    template <class U, int M>
    struct is_vec<class Vec<U, M>> : std::true_type {
    };

    /**
     * @brief Convenience variable template for is_vec, which decays the type to remove references
     * and cv-qualifiers
     */
    template <class A>
    inline constexpr bool is_vec_v = is_vec<std::decay_t<A>>::value;

    /**
     * @brief Type trait to extract the scalar type from a Vec, or return the type itself if it's
     * not a Vec.
     */
    template <class A>
    struct vec_scalar_type {
        using type = std::decay_t<A>;
    };

    /**
     * @brief Specialization of vec_scalar_type for Vec types, which defines the type member as the
     * scalar
     */
    template <class U, int M>
    struct vec_scalar_type<Vec<U, M>> {
        using type = U;
    };

    /**
     * @brief Convenience type alias for vec_scalar_type, which decays the type to remove references
     * and cv-qualifiers
     */
    template <class A>
    using vec_scalar_t = typename vec_scalar_type<std::decay_t<A>>::type;

    /**
     * @brief Type trait to count the number of scalar components in a type, which is 1 for non-Vec
     * types and M for Vec<U, M> types.
     */
    template <class A>
    struct component_count : std::integral_constant<size_t, 1> {
    };

    /**
     * @brief Specialization of component_count for Vec types, which defines the value member as the
     * number of components in the Vec
     */
    template <class U, int M>
    struct component_count<Vec<U, M>> : std::integral_constant<size_t, M> {
    };

    /**
     * @brief Convenience variable template for component_count, which decays the type to remove
     * references
     */
    template <class... Args>
    inline constexpr size_t component_sum_v =
        (component_count<std::decay_t<Args>>::value + ... + 0);

    /**
     * @brief Type trait to check if a type is either a scalar (arithmetic type that is not a Vec)
     * or a Vec.
     */
    template <class A>
    inline constexpr bool is_scalar_or_vec_v =
        (std::is_arithmetic_v<std::decay_t<A>> && !is_vec_v<A>) || is_vec_v<A>;

    /**
     * @brief Type trait to check if all types in a parameter pack are convertible to a specified
     * type T
     */
    template <class T, class... Args>
    inline constexpr bool all_convertible_to_v =
        (std::is_convertible_v<vec_scalar_t<Args>, T> && ...);

    /**
     * @brief Type trait to check if the parameter pack of arguments is NOT exactly one Vec<T, N>
     */
    template <class T, int N, class... Args>
    inline constexpr bool not_exact_vec_copy_v =
        !(sizeof...(Args) == 1 && (std::is_same_v<std::decay_t<Args>, Vec<T, N>> && ...));

    /**
     * @brief Helper function to append the components of an argument to a destination array
     */
    template <class T>
    constexpr void append_components(size_t& idx, T* dst, const T& v)
    {
        dst[idx++] = v;
    }

    /**
     * @brief Helper function to append the components of an argument to a destination array
     */
    template <class T, class S, std::enable_if_t<std::is_arithmetic_v<std::decay_t<S>>, int> = 0>
    constexpr void append_components(size_t& idx, T* dst, const S& v)
    {
        dst[idx++] = static_cast<T>(v);
    }

    /**
     * @brief Helper function to append the components of a Vec argument to a destination array
     */
    template <class T, class U, int M>
    constexpr void append_components(size_t& idx, T* dst, const Vec<U, M>& v)
    {
        for (int i = 0; i < M; ++i)
            dst[idx++] = static_cast<T>(v[i]);
    }

    /**
     * @brief Helper function to append the components of a Vec argument to a destination array
     */
    template <class T, class First, class... Rest>
    constexpr void append_all(size_t& idx, T* dst, const First& f, const Rest&... r)
    {
        append_components(idx, dst, f);
        if constexpr (sizeof...(Rest) > 0)
            append_all(idx, dst, r...);
    }

    /**
     * @brief A simple vector storage class template that provides a union of named components (x,
     * y, z, w)
     */
    template <typename T, int N>
    struct VecStorage {
        T data[N];
    };

    /**
     * @brief Specializations of VecStorage for 2D, 3D, and 4D vectors that provide named component
     * access
     */
    template <typename T>
    struct VecStorage<T, 2> {
        union {
            struct {
                T x, y;
            };
            T data[2];
        };
    };
    template <typename T>
    struct VecStorage<T, 3> {
        union {
            struct {
                T x, y, z;
            };
            T data[3];
        };
    };
    template <typename T>
    struct VecStorage<T, 4> {
        union {
            struct {
                T x, y, z, w;
            };
            T data[4];
        };
    };

    /**
     * @brief A simple vector class template that inherits from VecStorage to provide named
     * component access
     */
    template <typename T, int N>
    class VecBase
    {
    public:
        VecBase() = default;
        VecBase(const VecBase&) = default;
        VecBase& operator=(const VecBase& other) = default;
        ~VecBase() = default;
    };

    /**
     * @brief A simple vector class template that inherits from VecBase and VecStorage to provide
     * basic vector
     */
    template <typename T, int N>
    class Vec : public VecBase<T, N>, public VecStorage<T, N>
    {
    public:
        using value_type = T;
        static constexpr int kSize = N;

        Vec() = default;
        Vec(const Vec&) = default;
        Vec& operator=(const Vec&) = default;
        ~Vec() = default;

        constexpr T& operator[](int i) noexcept { return this->data[i]; }
        constexpr const T& operator[](int i) const noexcept { return this->data[i]; }
        explicit constexpr Vec(T fill)
        {
            for (int i = 0; i < N; ++i)
                this->data[i] = fill;
        }
        constexpr Vec(std::initializer_list<T> ilist)
        {
            assert(ilist.size() == static_cast<size_t>(N) && "initializer_list wrong size");
            int i = 0;
            for (const T& v : ilist)
                this->data[i++] = v;
        }
        template <class U,
                  class = std::enable_if_t<std::is_arithmetic_v<U> && std::is_convertible_v<U, T>>>
        explicit constexpr Vec(std::initializer_list<U> ilist)
        {
            assert(ilist.size() == static_cast<size_t>(N) && "initializer_list wrong size");
            int i = 0;
            for (const U& v : ilist)
                this->data[i++] = static_cast<T>(v);
        }
        Vec& operator=(std::initializer_list<T> ilist)
        {
            assert(ilist.size() == static_cast<size_t>(N) && "initializer_list wrong size");
            int i = 0;
            for (const T& v : ilist)
                this->data[i++] = v;
            return *this;
        }
        template <class U,
                  class = std::enable_if_t<std::is_arithmetic_v<U> && std::is_convertible_v<U, T>>>
        Vec& operator=(std::initializer_list<U> ilist)
        {
            assert(ilist.size() == static_cast<size_t>(N) && "initializer_list wrong size");
            int i = 0;
            for (const U& v : ilist)
                this->data[i++] = static_cast<T>(v);
            return *this;
        }
        template <
            class... Args,
            typename = std::enable_if_t<
                (sizeof...(Args) > 0) && (component_sum_v<Args...> == static_cast<size_t>(N)) &&
                ((is_scalar_or_vec_v<Args>) && ...) && (all_convertible_to_v<T, Args...>) &&
                (not_exact_vec_copy_v<T, N, Args...>)>>
        constexpr explicit Vec(const Args&... args)
        {
            size_t i = 0;
            append_all(i, this->data, args...);
        }
        constexpr bool operator==(const Vec& other) const noexcept
        {
            for (int i = 0; i < N; ++i)
                if (this->data[i] != other.data[i])
                    return false;
            return true;
        }
        constexpr bool operator!=(const Vec& other) const noexcept { return !(*this == other); }
    };

    //---- Basic vector math ------------------------------------------------------------
    template <typename T, int N>
    Vec<T, N> operator+(const Vec<T, N>& a, const Vec<T, N>& b)
    {
        Vec<T, N> r;
        for (int i = 0; i < N; ++i)
            r.data[i] = a.data[i] + b.data[i];
        return r;
    }
    template <typename T, int N>
    Vec<T, N> operator-(const Vec<T, N>& a, const Vec<T, N>& b)
    {
        Vec<T, N> r;
        for (int i = 0; i < N; ++i)
            r.data[i] = a.data[i] - b.data[i];
        return r;
    }

    template <typename T, int N, typename S, std::enable_if_t<std::is_arithmetic_v<S>, int> = 0>
    Vec<T, N> operator*(const Vec<T, N>& v, S scalar)
    {
        Vec<T, N> r;
        for (int i = 0; i < N; ++i)
            r.data[i] = v.data[i] * static_cast<T>(scalar);
        return r;
    }
    template <typename T, int N, typename S, std::enable_if_t<std::is_arithmetic_v<S>, int> = 0>
    Vec<T, N> operator*(S scalar, const Vec<T, N>& v)
    {
        Vec<T, N> r;
        for (int i = 0; i < N; ++i)
            r.data[i] = v.data[i] * static_cast<T>(scalar);
        return r;
    }
    template <typename T, int N, typename S, std::enable_if_t<std::is_arithmetic_v<S>, int> = 0>
    Vec<T, N> operator/(const Vec<T, N>& v, S scalar)
    {
        Vec<T, N> r;
        for (int i = 0; i < N; ++i)
            r.data[i] = v.data[i] / static_cast<T>(scalar);
        return r;
    }
    template <typename T, int N>
    Vec<T, N> operator-(const Vec<T, N>& v)
    {
        Vec<T, N> r;
        for (int i = 0; i < N; ++i)
            r.data[i] = -v.data[i];
        return r;
    }
} // namespace chai