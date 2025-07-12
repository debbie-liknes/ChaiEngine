#pragma once
#include <memory>
#include <VecBase.h>
#include <ChaiMathExport.h>

namespace chai
{
    template<typename T, int C, int R>
    struct MatImpl;

    template<typename T, int C, int R>
    class MatColumnRef;

    template<typename T, int C, int R>
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
        MatColumnRef<T, C, R> operator[](int i);
        const Vec<T, R> operator[](int i) const;

        // Standard comparison operators
        bool operator==(const Mat& other) const;

    private:
        std::unique_ptr<MatImpl<T, C, R>> impl_;
    };

    template<typename T> using Mat2x2T = Mat<T, 2, 2>;
    template<typename T> using Mat3x3T = Mat<T, 3, 3>;

    using Mat2x2f = Mat2x2T<float>;
    using Mat3x3f = Mat3x3T<float>;
    using Mat2x2d = Mat2x2T<double>;
    using Mat3x3d = Mat3x3T<double>;

    using Mat2 = Mat2x2f;
    using Mat3 = Mat3x3f;

    template<typename T, int C, int R>
    CHAIMATH_EXPORT Mat<T, C, R> operator*(const Mat<T, C, R>& a, const Mat<T, C, R>& b);
}