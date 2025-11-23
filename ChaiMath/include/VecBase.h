#pragma once
#include <MathIncludes.h>
#include <type_traits>
#include <memory>
#include <initializer_list>
#include <cassert>

namespace chai
{
template <typename T, int N>
class Vec;

//type traits
template <class>
struct is_vec : std::false_type {};

template <class U, int M>
struct is_vec<class Vec<U, M>> : std::true_type {};

template <class A>
inline constexpr bool is_vec_v = is_vec<std::decay_t<A>>::value;

template <class A>
struct vec_scalar_type
{
    using type = std::decay_t<A>;
};

template <class U, int M>
struct vec_scalar_type<Vec<U, M>>
{
    using type = U;
};

template <class A>
using vec_scalar_t = typename vec_scalar_type<std::decay_t<A>>::type;

template <class A>
struct component_count : std::integral_constant<size_t, 1> {};

template <class U, int M>
struct component_count<Vec<U, M>> : std::integral_constant<size_t, M> {};

template <class... Args>
inline constexpr size_t component_sum_v = (component_count<std::decay_t<Args>>::value + ... + 0);

template <class A>
inline constexpr bool is_scalar_or_vec_v =
    (std::is_arithmetic_v<std::decay_t<A>> && !is_vec_v<A>) || is_vec_v<A>;

template <class T, class... Args>
inline constexpr bool all_convertible_to_v =
    (std::is_convertible_v<vec_scalar_t<Args>, T> && ...);

template <class T, int N, class... Args>
inline constexpr bool not_exact_vec_copy_v =
    !(sizeof...(Args) == 1 &&
      (std::is_same_v<std::decay_t<Args>, Vec<T, N>> && ...));

//append to vec
template <class T>
constexpr void append_components(size_t& idx, T* dst, const T& v) { dst[idx++] = v; }

template <class T, class S, std::enable_if_t<std::is_arithmetic_v<std::decay_t<S>>, int> = 0>
constexpr void append_components(size_t& idx, T* dst, const S& v)
{
    dst[idx++] = static_cast<T>(v);
}

template <class T, class U, int M>
constexpr void append_components(size_t& idx, T* dst, const Vec<U, M>& v)
{
    for (int i = 0; i < M; ++i) {
        dst[idx++] = static_cast<T>(v[i]); // uses Vec::operator[]; or use v.data[i]
    }
}

template <class T, class First, class... Rest>
constexpr void append_all(size_t& idx, T* dst, const First& f, const Rest&... r)
{
    append_components(idx, dst, f);
    if constexpr (sizeof...(Rest) > 0)
        append_all(idx, dst, r...);
}

//storage for vec data
//need to maintain alignment and have named memebers
template <typename T, int N>
struct VecStorage
{
    T data[N];
};

//vec types with named members for small N
// common to need .x .y .z .w access

template <typename T>
struct VecStorage<T, 2>
{
    union
    {
        struct
        {
            T x, y;
        };

        T data[2];
    };
};

template <typename T>
struct VecStorage<T, 3>
{
    union
    {
        struct
        {
            T x, y, z;
        };

        T data[3];
    };
};

template <typename T>
struct VecStorage<T, 4>
{
    union
    {
        struct
        {
            T x, y, z, w;
        };

        T data[4];
    };
};

//Vec base class to allow specialization later
template <typename T, int N>
class VecBase
{
public:
    VecBase() = default;
    VecBase(const VecBase&) = default;
    VecBase& operator=(const VecBase& other) = default;
    ~VecBase() = default;
};

template <typename T, int N>
class Vec : public VecBase<T, N>, public VecStorage<T, N>
{
public:
    using value_type = T;
    static constexpr int kSize = N;

    // Special members
    Vec() = default;
    Vec(const Vec&) = default;
    Vec& operator=(const Vec&) = default;
    ~Vec() = default;

    constexpr T& operator[](int i) noexcept { return this->data[i]; }
    constexpr const T& operator[](int i) const noexcept { return this->data[i]; }

    // Fill in constructor
    explicit constexpr Vec(T fill)
    {
        for (int i = 0; i < N; ++i)
            this->data[i] = fill;
    }

    //initializer_list constructors
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

    // assignment
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

    template <class... Args,
              typename = std::enable_if_t<
                  (sizeof...(Args) > 0) &&
                  (component_sum_v<Args...> == static_cast<size_t>(N)) &&
                  ((is_scalar_or_vec_v<Args>) && ...) &&
                  (all_convertible_to_v<T, Args...>) &&
                  (not_exact_vec_copy_v<T, N, Args...>)
              >>
    constexpr explicit Vec(const Args&... args)
    {
        size_t i = 0;
        append_all(i, this->data, args...);
    }

    constexpr bool operator==(const Vec& other) const noexcept
    {
        for (int i = 0; i < N; ++i) {
            if (this->data[i] != other.data[i])
                return false;
        }
        return true;
    }

    // Inequality operator
    constexpr bool operator!=(const Vec& other) const noexcept
    {
        return !(*this == other);
    }
};

template class Vec<float, 2>;
template class Vec<float, 3>;
template class Vec<float, 4>;
template class Vec<double, 2>;
template class Vec<double, 3>;
template class Vec<double, 4>;

//free function operators

template <typename T, int N>
Vec<T, N> operator+(const Vec<T, N>& a, const Vec<T, N>& b)
{
    Vec<T, N> result;
    for (int i = 0; i < N; ++i) {
        result.data[i] = a.data[i] + b.data[i];
    }
    return result;
}

template <typename T, int N>
Vec<T, N> operator-(const Vec<T, N>& a, const Vec<T, N>& b)
{
    Vec<T, N> result;
    for (int i = 0; i < N; ++i) {
        result.data[i] = a.data[i] - b.data[i];
    }
    return result;
}

template <typename T, int N>
Vec<T, N> operator*(const Vec<T, N>& v, const T scalar)
{
    Vec<T, N> result;
    for (int i = 0; i < N; ++i) {
        result.data[i] = v.data[i] * scalar;
    }
    return result;
}

template <typename T, int N>
Vec<T, N> operator*(const T scalar, const Vec<T, N>& v)
{
    Vec<T, N> result;
    for (int i = 0; i < N; ++i) {
        result.data[i] = v.data[i] * scalar;
    }
    return result;
}

template <typename T, int N>
Vec<T, N> operator/(const Vec<T, N>& v, const T scalar)
{
    Vec<T, N> result;
    for (int i = 0; i < N; ++i) {
        result.data[i] = v.data[i] / scalar;
    }
    return result;
}

template <typename T, int N>
Vec<T, N> operator-(const Vec<T, N>& v)
{
    Vec<T, N> result;
    for (int i = 0; i < N; ++i) {
        result.data[i] = -v.data[i];
    }
    return result;
}
}