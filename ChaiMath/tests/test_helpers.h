#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ChaiMath.h>

namespace chai
{
    using chai::EPSILON_F;

    template <typename T, int N>
    ::testing::AssertionResult VecNear(const Vec<T, N>& a, const Vec<T, N>& b, T epsilon)
    {
        if (std::fabs(a.x - b.x) > epsilon) return ::testing::AssertionFailure() << "x mismatch";
        if (std::fabs(a.y - b.y) > epsilon) return ::testing::AssertionFailure() << "y mismatch";
        if (std::fabs(a.z - b.z) > epsilon) return ::testing::AssertionFailure() << "z mismatch";
        return ::testing::AssertionSuccess();
    }

    template <typename T, int C, int R>
    ::testing::AssertionResult MatNear(const Mat<T, C, R>& a, const Mat<T, C, R>& b, T epsilon)
    {
        for (int col = 0; col < C; ++col)
        {
            for (int row = 0; row < R; ++row)
            {
                if (std::fabs(a[col][row] - b[col][row]) > epsilon)
                    return ::testing::AssertionFailure()
                        << "Mismatch at [" << col << "][" << row << "]: "
                        << a[col][row] << " vs " << b[col][row];
            }
        }
        return ::testing::AssertionSuccess();
    }

    template <typename T>
    bool nearlyEqual(T a, T b, T eps = EPSILON_F)
    {
        return std::fabs(a - b) < eps;
    }

    template <typename T>
    ::testing::AssertionResult QuatNear(const Quaternion<T>& a, const Quaternion<T>& b, T eps = EPSILON_F)
    {
        if (!nearlyEqual(a.x, b.x, eps)) return ::testing::AssertionFailure() << "x mismatch";
        if (!nearlyEqual(a.y, b.y, eps)) return ::testing::AssertionFailure() << "y mismatch";
        if (!nearlyEqual(a.z, b.z, eps)) return ::testing::AssertionFailure() << "z mismatch";
        if (!nearlyEqual(a.w, b.w, eps)) return ::testing::AssertionFailure() << "w mismatch";
        return ::testing::AssertionSuccess();
    }
}