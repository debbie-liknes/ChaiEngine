#include <ChaiMatrix.h>
#include "internal/glm_internal.h"
#include <stdexcept>

namespace chai
{
    // MatImpl implementation using GLM matrices
    template<typename T, int N>
    struct MatImpl 
    {
        using GLMMatrix = typename std::conditional<
            N == 2, glm::mat<2, 2, T>,
            typename std::conditional<
            N == 3, glm::mat<3, 3, T>,
            typename std::conditional<
            N == 4, glm::mat<4, 4, T>,
            glm::mat<N, N, T>
            >::type
            >::type
        >::type;

        GLMMatrix matrix;

        MatImpl() : matrix(T(0)) 
        {
        }

        template<typename... Args>
        MatImpl(Args... args) {
            static_assert(sizeof...(args) == N * N, "Number of arguments must match matrix size");
            T values[] = { static_cast<T>(args)... };
            // GLM matrices are column-major, so we need to transpose
            for (int col = 0; col < N; ++col) 
            {
                for (int row = 0; row < N; ++row) 
                {
                    matrix[col][row] = values[row * N + col];
                }
            }
        }

        MatImpl(std::initializer_list<T> init) 
        {
            if (init.size() != N * N) 
            {
                throw std::invalid_argument("Initializer list size must match matrix size");
            }
            auto it = init.begin();
            // GLM matrices are column-major, so we need to transpose
            for (int col = 0; col < N; ++col) 
            {
                for (int row = 0; row < N; ++row) 
                {
                    matrix[col][row] = *(it + row * N + col);
                }
            }
        }

        MatImpl(const MatImpl& other) : matrix(other.matrix) 
        {
        }

        MatImpl& operator=(const MatImpl& other) 
        {
            if (this != &other) 
            {
                matrix = other.matrix;
            }
            return *this;
        }

        Vec<T, N>& operator[](int i)
        {
            return matrix[i];
        }

        const Vec<T, N>& operator[](int i) const
        {
            return matrix[i];
        }

        bool operator==(const MatImpl& other) const 
        {
            // GLM provides comparison operators
            return matrix == other.matrix;
        }
    };

    template<typename T, int N>
    Mat<T, N>::Mat() : impl_(std::make_unique<MatImpl<T, N>>()) 
    {
    }

    template<typename T, int N>
    Mat<T, N>::~Mat() = default;

    template<typename T, int N>
    Mat<T, N>::Mat(const Mat& other) : impl_(std::make_unique<MatImpl<T, N>>(*other.impl_)) 
    {
    }

    template<typename T, int N>
    Mat<T, N>& Mat<T, N>::operator=(const Mat& other) 
    {
        if (this != &other) 
        {
            *impl_ = *other.impl_;
        }
        return *this;
    }

    template<typename T, int N>
    Mat<T, N>::Mat(Mat&& other) noexcept : impl_(std::move(other.impl_)) 
    {
    }

    template<typename T, int N>
    Mat<T, N>& Mat<T, N>::operator=(Mat&& other) noexcept 
    {
        if (this != &other) 
        {
            impl_ = std::move(other.impl_);
        }
        return *this;
    }

    template<typename T, int N>
    template<typename... Args>
    Mat<T, N>::Mat(Args... args) : impl_(std::make_unique<MatImpl<T, N>>(args...)) 
    {
    }

    template<typename T, int N>
    Mat<T, N>::Mat(std::initializer_list<T> init) : impl_(std::make_unique<MatImpl<T, N>>(init)) 
    {
    }

    template<typename T, int N>
    bool Mat<T, N>::operator==(const Mat& other) const 
    {
        return *impl_ == *other.impl_;
    }

    template<typename T, int N>
    Mat<T, N> operator*(const Mat<T, N> a, const Mat<T, N>& b)
    {
		return a.impl_ * b.impl_->matrix;
    }
}