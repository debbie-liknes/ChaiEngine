#pragma once
#include <memory>
#include <VecBase.h>
#include <ChaiMathExport.h>

namespace chai
{
    namespace internal
    {
        template<typename T, int C, int R>
        struct MatAccess;
    }

    template<typename T, int C, int R>
    struct MatImpl;

    template<typename T, int C, int R>
    class Mat;

    template<typename T, int C, int R>
    class CHAIMATH_EXPORT MatColumnRef 
    {
    public:
        MatColumnRef(const MatColumnRef&);                // rule of 5
        MatColumnRef& operator=(const MatColumnRef&);
        MatColumnRef(MatColumnRef&&) noexcept;
        MatColumnRef& operator=(MatColumnRef&&) noexcept;
        ~MatColumnRef();

        // Read access
        operator Vec<T, R>() const;

        // Write access
        MatColumnRef& operator=(const Vec<T, R>& v);

        T operator[](int i) const;
        T& operator[](int i);

    private:
        class Impl;
        std::unique_ptr<Impl> impl_;

        // Only Mat can create this
        template<typename, int, int>
        friend class Mat;

        // Private constructor only usable from Mat
        explicit MatColumnRef(void* vecPtr); // forward-erased
    };

    template<typename T>
    Mat<T, 4, 4> translate(const Mat<T, 4, 4>& mat, const Vec<T, 3>& vec);

    template<typename T>
    Mat<T, 4, 4> scale(const Mat<T, 4, 4>& mat, const Vec<T, 3>& vec);

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
        Mat(Args... args);

        Mat(std::initializer_list<T> init);

        // Component access
        MatColumnRef<T, C, R> operator[](int i);
        const Vec<T, R> operator[](int i) const;

        // Standard comparison operators
        bool operator==(const Mat& other) const;

		static Mat<T, C, R> identity();

    private:
        std::unique_ptr<MatImpl<T, C, R>> impl_;

        template<typename, int, int>
        friend struct internal::MatAccess;
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
    Mat<T, C, R> operator*(const Mat<T, C, R>& a, const Mat<T, C, R>& b);
}