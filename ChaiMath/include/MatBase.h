#pragma once
#include <VecBase.h>
#include <cassert>

#ifndef CHAI_ROW_MAJOR
#define CHAI_ROW_MAJOR 0
#endif

namespace chai
{
    template <typename T, int R, int C>
    class Mat
    {
    public:
        using value_type = T;
        static constexpr int Rows = R;
        static constexpr int Cols = C;
        static constexpr int Elements = R * C;

        // contiguous storage
        T m[Elements];

        // Proxy for returning a vec when getting a row by index
        class RowProxy
        {
        public:
            explicit constexpr RowProxy(T* p) noexcept : p_(p)
            {
            }

            constexpr T& operator[](int c) noexcept { return p_[c]; }
            constexpr const T& operator[](int c) const noexcept { return p_[c]; }

            RowProxy& operator=(const Vec<T, C>& rhs) noexcept
            {
                for (int c = 0; c < C; ++c)
                    p_[c] = rhs[c];
                return *this;
            }

            RowProxy& operator=(std::initializer_list<T> il)
            {
                assert(il.size() == static_cast<size_t>(C));
                int i = 0;
                for (const T& v : il)
                    p_[i++] = v;
                return *this;
            }

            constexpr operator Vec<T, C>() const noexcept
            {
                Vec<T, C> out{};
                for (int c = 0; c < C; ++c)
                    out[c] = p_[c];
                return out;
            }

            constexpr T* data() noexcept { return p_; }
            constexpr const T* data() const noexcept { return p_; }
            constexpr T* begin() noexcept { return p_; }
            constexpr const T* begin() const noexcept { return p_; }
            constexpr T* end() noexcept { return p_ + C; }
            constexpr const T* end() const noexcept { return p_ + C; }

        private:
            T* p_;
        };

        class ConstRowProxy
        {
        public:
            explicit constexpr ConstRowProxy(const T* p) noexcept : p_(p)
            {
            }

            constexpr const T& operator[](int c) const noexcept { return p_[c]; }

            constexpr operator Vec<T, C>() const noexcept
            {
                Vec<T, C> out{};
                for (int c = 0; c < C; ++c)
                    out[c] = p_[c];
                return out;
            }

            constexpr const T* data() const noexcept { return p_; }
            constexpr const T* begin() const noexcept { return p_; }
            constexpr const T* end() const noexcept { return p_ + C; }

        private:
            const T* p_;
        };

        // Constructors

        Mat() = default;
        Mat(const Mat&) = default;
        Mat& operator=(const Mat&) = default;
        ~Mat() = default;

        explicit constexpr Mat(T v)
        {
            for (int i = 0; i < Elements; ++i)
                m[i] = v;
        }

        constexpr Mat(std::initializer_list<T> ilist)
        {
            if (ilist.size() == 1 && (*ilist.begin()) == T(1))
            {
                *this = Mat::identity();
                return;
            }

            assert(ilist.size() == static_cast<size_t>(Elements) && "Mat init wrong size");
            int i = 0;
            for (const T& v : ilist)
                m[i++] = v;
        }

        // Initializer list that can accept arithmetic types
        template <class U, class = std::enable_if_t<
                      std::is_arithmetic_v<U> && std::is_convertible_v<U, T>>>
        explicit constexpr Mat(std::initializer_list<U> ilist)
        {
            assert(ilist.size() == static_cast<size_t>(Elements) && "Mat init wrong size");
            int i = 0;
            for (const U& v : ilist)
                m[i++] = static_cast<T>(v);
        }

        // GLM-style ctor
        template <class... Args,
                  class = std::enable_if_t<
                      (sizeof...(Args) > 0) &&
                      (component_sum_v < Args...>== static_cast<size_t>
        (Elements)
        )
        &&
        ((is_scalar_or_vec_v<Args>)
        &&
        ...
        )
        &&
        (all_convertible_to_v<T, Args...>)
        >
        >
        constexpr explicit Mat(const Args&... args)
        {
            size_t i = 0;
            append_all(i, m, args...);
        }

        constexpr T& operator()(int r, int c) noexcept { return m[index(r, c)]; }
        constexpr const T& operator()(int r, int c) const noexcept { return m[index(r, c)]; }

        constexpr RowProxy operator[](int i) noexcept {
#if CHAI_ROW_MAJOR
            // row-major: i = row index
            return RowProxy(&m[index(i, 0)]);
#else
            // column-major: i = column index
            return RowProxy(&m[index(0, i)]); // start of column i
#endif
        }

        constexpr ConstRowProxy operator[](int i) const noexcept {
#if CHAI_ROW_MAJOR
            return ConstRowProxy(&m[index(i, 0)]);
#else
            return ConstRowProxy(&m[index(0, i)]);
#endif
        }


        constexpr T* data() noexcept { return m; }
        constexpr const T* data() const noexcept { return m; }
        constexpr T* begin() noexcept { return m; }
        constexpr const T* begin() const noexcept { return m; }
        constexpr T* end() noexcept { return m + Elements; }
        constexpr const T* end() const noexcept { return m + Elements; }


        static constexpr Mat zero() noexcept
        {
            Mat z{};
            for (int i = 0; i < Elements; ++i)
                z.m[i] = T(0);
            return z;
        }

        static constexpr Mat identity() noexcept
        {
            static_assert(R == C, "identity() requires square matrix");
            Mat I = zero();
            for (int i = 0; i < R; ++i)
                I(i, i) = T(1);
            return I;
        }

        // comparison
        constexpr bool operator==(const Mat& o) const noexcept
        {
            for (int i = 0; i < Elements; ++i)
                if (m[i] != o.m[i])
                    return false;
            return true;
        }

        constexpr bool operator!=(const Mat& o) const noexcept { return !(*this == o); }

        // arithmetic
        constexpr Mat& operator+=(const Mat& b) noexcept
        {
            for (int i = 0; i < Elements; ++i)
                m[i] += b.m[i];
            return *this;
        }

        constexpr Mat& operator-=(const Mat& b) noexcept
        {
            for (int i = 0; i < Elements; ++i)
                m[i] -= b.m[i];
            return *this;
        }

        constexpr Mat& operator*=(T s) noexcept
        {
            for (int i = 0; i < Elements; ++i)
                m[i] *= s;
            return *this;
        }

        constexpr Mat& operator/=(T s) noexcept
        {
            for (int i = 0; i < Elements; ++i)
                m[i] /= s;
            return *this;
        }

        friend constexpr Mat operator+(Mat a, const Mat& b) noexcept
        {
            a += b;
            return a;
        }

        friend constexpr Mat operator-(Mat a, const Mat& b) noexcept
        {
            a -= b;
            return a;
        }

        friend constexpr Mat operator*(Mat a, T s) noexcept
        {
            a *= s;
            return a;
        }

        friend constexpr Mat operator*(T s, Mat a) noexcept
        {
            a *= s;
            return a;
        }

        friend constexpr Mat operator/(Mat a, T s) noexcept
        {
            a /= s;
            return a;
        }

        //Matrix math ops

        template <int K>
        friend constexpr Mat<T, R, K> operator*(const Mat& A, const Mat<T, C, K>& B) noexcept
        {
            Mat<T, R, K> out = Mat<T, R, K>::zero();
            for (int r = 0; r < R; ++r)
                for (int k = 0; k < K; ++k)
                {
                    T acc = T(0);
                    for (int c = 0; c < C; ++c)
                        acc += A(r, c) * B(c, k);
                    out(r, k) = acc;
                }
            return out;
        }

        friend constexpr Vec<T, R> operator*(const Mat& A, const Vec<T, C>& x) noexcept
        {
            Vec<T, R> out{};
            for (int r = 0; r < R; ++r)
            {
                T acc = T(0);
                for (int c = 0; c < C; ++c)
                    acc += A(r, c) * x[c];
                out[r] = acc;
            }
            return out;
        }

        friend constexpr Vec<T, C> operator*(const Vec<T, R>& x, const Mat& A) noexcept
        {
            Vec<T, C> out{};
            for (int c = 0; c < C; ++c)
            {
                T acc = T(0);
                for (int r = 0; r < R; ++r)
                    acc += x[r] * A(r, c);
                out[c] = acc;
            }
            return out;
        }

        constexpr Mat<T, C, R> transpose() const noexcept
        {
            Mat<T, C, R> t{};
            for (int r = 0; r < R; ++r)
                for (int c = 0; c < C; ++c)
                    t(c, r) = (*this)(r, c);
            return t;
        }

        constexpr Mat<T, R, C> inverse() const noexcept
        {
            static_assert(R == C, "inverse() requires square matrix");
            Mat<T, R, C> inv = Mat<T, R, C>::identity();
            Mat<T, R, C> mcopy = *this;
            for (int i = 0; i < R; ++i)
            {
                // Find pivot
                T pivot = mcopy(i, i);
                if (pivot == T(0))
                {
                    // Singular matrix, return identity as fallback
                    return Mat<T, R, C>::identity();
                }
                T invPivot = T(1) / pivot;
                // Normalize pivot row
                for (int c = 0; c < C; ++c)
                {
                    mcopy(i, c) *= invPivot;
                    inv(i, c) *= invPivot;
                }
                // Eliminate other rows
                for (int r = 0; r < R; ++r)
                {
                    if (r != i)
                    {
                        T factor = mcopy(r, i);
                        for (int c = 0; c < C; ++c)
                        {
                            mcopy(r, c) -= factor * mcopy(i, c);
                            inv(r, c) -= factor * inv(i, c);
                        }
                    }
                }
            }
            return inv;
        }

    private:
        static constexpr int index(int r, int c) noexcept
        {
#if CHAI_ROW_MAJOR
            return r * C + c;
#else
            return c * R + r;
#endif
        }
    };
}