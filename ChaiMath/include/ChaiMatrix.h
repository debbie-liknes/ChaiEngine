#pragma once
#include <memory>
#include <VecBase.h>
#include <ChaiMathExport.h>

namespace chai
{
    template<typename T, int N>
    struct MatImpl;

    template<typename T, int N>
    class CHAIMATH_EXPORT Mat
    {
    public:
        Mat();
        ~Mat();

        // Copy constructor and assignment
        Mat(const Mat& other);
        Mat& operator=(const Mat& other);

        // Move constructor and assignment
        Mat(Mat&& other) noexcept;
        Mat& operator=(Mat&& other) noexcept;

        // Constructor for different dimensions
        template<typename... Args>
        explicit Mat(Args... args);

        Mat(std::initializer_list<T> init);

        // Component access
        Vec<T, N>& operator[](int i);
        const Vec<T, N>& operator[](int i) const;

        // Standard comparison operators
        bool operator==(const Mat& other) const;

    private:
        std::unique_ptr<MatImpl<T, N>> impl_;
    };

    template<typename T> using Mat2T = Mat<T, 2>;
    template<typename T> using Mat3T = Mat<T, 3>;
    template<typename T> using Mat4T = Mat<T, 4>;

    using Mat2f = Mat2T<float>;
    using Mat3f = Mat3T<float>;
    using Mat4f = Mat4T<float>;
    using Mat2d = Mat2T<double>;
    using Mat3d = Mat3T<double>;
    using Mat4d = Mat4T<double>;

    using Mat2 = Mat2f;
    using Mat3 = Mat3f;
    using Mat4 = Mat4f;

    template<typename T, int N>
    CHAIMATH_EXPORT Mat<T, N> operator*(const Mat<T, N> a, const Mat<T, N>& b);
}