#pragma once
#include <ChaiMathExport.h>
#include <memory>
#include <VecBase.h>

namespace chai
{
//#define DEFINE_MAT_CONSTRUCTORS(C, R) \
//    Mat() { std::fill(data, data + C * R, T(0)); } \
//    template<typename... Args> \
//    Mat(Args... args) { \
//        static_assert(sizeof...(args) <= C * R, "Too many arguments"); \
//        T values[] = {T(args)...}; \
//        std::copy(values, values + sizeof...(args), data); \
//        std::fill(data + sizeof...(args), data + C * R, T(0)); \
//    } \
//    Mat(std::initializer_list<T> init) { \
//        auto it = init.begin(); \
//        int i = 0; \
//        for(; it != init.end() && i < C * R; ++it, ++i) { \
//            data[i] = *it; \
//        } \
//        for(; i < C * R; ++i) { \
//            data[i] = T(0); \
//        } \
//    }
//
//#define DEFINE_MAT_OPERATORS(C, R) \
//    inline Vec<T, C>& operator[](int row) { \
//        return reinterpret_cast<Vec<T, C>&>(data[row * C]); \
//    } \
//    inline const Vec<T, C>& operator[](int row) const { \
//        return reinterpret_cast<const Vec<T, C>&>(data[row * C]); \
//    } \
//    inline Mat& operator+=(const Mat& other) { \
//        for(int i = 0; i < C * R; ++i) data[i] += other.data[i]; \
//        return *this; \
//    } \
//    template<int CC = C, int RR = R> \
//    static typename std::enable_if<CC == RR, Mat>::type identity() { \
//        Mat result{}; \
//        for(int i = 0; i < C; ++i) { \
//            result.data[i * C + i] = T(1); \
//        } \
//        return result; \
//    }\
//    bool operator==(const Mat& other) const {\
//        for (int i = 0; i < C * R; ++i) \
//            if (data[i] != other.data[i]) return false; \
//                return true; \
//    }
//
//    template<typename T, int C, int R>
//    class CHAIMATH_EXPORT MatBase
//    {
//    public:
//        MatBase& operator=(const MatBase& other) = default;
//    };
//
//    template<typename T, int C, int R>
//    class CHAIMATH_EXPORT Mat : public MatBase<T, C, R>
//    {
//    public:
//        Mat& operator=(const Mat& other) = default;
//    };
//
//    template<typename T, int C, int R>
//    Mat<T, C, R> operator*(const Mat<T, C, R>& a, const Mat<T, C, R>& b)
//    {
//        Mat<T, C, R> result{};
//
//        for (int col = 0; col < C; ++col) 
//        {
//            for (int row = 0; row < R; ++row) 
//            {
//                T sum = T{};
//                for (int k = 0; k < C; ++k) 
//                {
//                    sum += a[k][row] * b[col][k];
//                }
//                result[col][row] = sum;
//            }
//        }
//
//        return result;
//    }
}