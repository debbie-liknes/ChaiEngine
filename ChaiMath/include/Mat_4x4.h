#pragma once
#include <ChaiMathExport.h>
#include <ChaiMatrix.h>

namespace chai
{
    template<typename T>
    class CHAIMATH_EXPORT Mat<T, 4, 4>
    {
    private:
        std::unique_ptr<MatImpl<T, 4, 4>> impl_;

        template<typename U, int C, int R>
        friend class MatColumnRef;

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
        Mat(Args... args);

        Mat(std::initializer_list<T> init);

        // Component access
        MatColumnRef<T, 4, 4> operator[](int i);
        const Vec<T, 4> operator[](int i) const;

        // Standard comparison operators
        bool operator==(const Mat& other) const;

        static Mat<T, 4, 4> identity();
    };

    template<typename T> using Mat4x4T = Mat<T, 4, 4>;
    using Mat4x4f = Mat4x4T<float>;
	using Mat4x4d = Mat4x4T<double>;

    //default to float
    using Mat4 = Mat4x4f;
}