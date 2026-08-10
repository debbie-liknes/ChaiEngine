/**
 * @file MathIncludes.h
 * @brief A header file that includes common mathematical constants and utilities.
 */
#pragma once
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <type_traits>

#if defined(__cpp_lib_math_constants) // C++20 <numbers>
#include <numbers>
#endif

namespace chai::math
{
#if defined(__cpp_lib_math_constants)
    template <typename T>
    inline constexpr T pi_v = std::numbers::pi_v<T>;
#else
    template <typename T>
    inline constexpr T pi_v = static_cast<T>(3.141592653589793238462643383279502884L);
#endif
    inline constexpr float pi = pi_v<float>;
    inline constexpr double pid = pi_v<double>;
} // namespace chai
