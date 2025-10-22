#pragma once
#include <VecBase.h>
#include <MathIncludes.h>
#include <cassert>

#ifndef CHAI_ROW_MAJOR
#define CHAI_ROW_MAJOR 1
#endif

namespace chai
{

    template<typename T, int R, int C>
    class Mat 
    {
    public:
        using value_type = T;
        static constexpr int Rows = R;
        static constexpr int Cols = C;
        static constexpr int Elements = R * C;

        // contiguous storage
        T m[Elements];

        class RowProxy 
        {
        public:
            explicit constexpr RowProxy(T* p) noexcept : p_(p) {}
            constexpr       T& operator[](int c)       noexcept { return p_[c]; }
            constexpr const T& operator[](int c) const noexcept { return p_[c]; }

            // assign from Vec<T,C>
            RowProxy& operator=(const Vec<T, C>& rhs) noexcept 
            {
                for (int c = 0; c < C; ++c) p_[c] = rhs[c];
                return *this;
            }
            // assign from initializer_list (strict)
            RowProxy& operator=(std::initializer_list<T> il) 
            {
                assert(il.size() == (size_t)C);
                int i = 0; for (const T& v : il) p_[i++] = v; return *this;
            }
            // implicit readback as Vec<T,C>
            constexpr operator Vec<T, C>() const noexcept 
            {
                Vec<T, C> out{};
                for (int c = 0; c < C; ++c) out[c] = p_[c];
                return out;
            }
            // iterators / data
            constexpr       T* data()       noexcept { return p_; }
            constexpr const T* data() const noexcept { return p_; }
            constexpr       T* begin()       noexcept { return p_; }
            constexpr const T* begin() const noexcept { return p_; }
            constexpr       T* end()         noexcept { return p_ + C; }
            constexpr const T* end()   const noexcept { return p_ + C; }
        private:
            T* p_;
        };

        class ConstRowProxy 
        {
        public:
            explicit constexpr ConstRowProxy(const T* p) noexcept : p_(p) {}
            constexpr const T& operator[](int c) const noexcept { return p_[c]; }
            constexpr operator Vec<T, C>() const noexcept 
            {
                Vec<T, C> out{};
                for (int c = 0; c < C; ++c) out[c] = p_[c];
                return out;
            }
            constexpr const T* data() const noexcept { return p_; }
            constexpr const T* begin() const noexcept { return p_; }
            constexpr const T* end()   const noexcept { return p_ + C; }
        private:
            const T* p_;
        };

        Mat() = default;
        Mat(const Mat&) = default;
        Mat& operator=(const Mat&) = default;
        ~Mat() = default;

        explicit constexpr Mat(T v) 
        {
            for (int i = 0; i < Elements; ++i) m[i] = v;
        }

        constexpr Mat(std::initializer_list<T> ilist) 
        {
            if (ilist.size() == 1 && (*ilist.begin()) == T(1)) 
            {
                for (int r = 0; r < R; ++r)
                    for (int c = 0; c < C; ++c)
                        m[r * C + c] = (r == c ? T(1) : T(0));
                return;
            }

            assert(ilist.size() == static_cast<size_t>(Elements) && "Mat init wrong size");
            int i = 0; for (const T& v : ilist) m[i++] = v;
        }

        // Relaxed initializer_list (accept arithmetic types; explicit)
        template<class U, class = std::enable_if_t<std::is_arithmetic_v<U>&& std::is_convertible_v<U, T>>>
        explicit constexpr Mat(std::initializer_list<U> ilist) 
        {
            assert(ilist.size() == static_cast<size_t>(Elements) && "Mat init wrong size");
            int i = 0; for (const U& v : ilist) m[i++] = static_cast<T>(v);
        }

        // Flexible “GLM-style” ctor: any mix of scalars and Vecs whose total components == R*C.
        template<class... Args,
            class = std::enable_if_t<
            (sizeof...(Args) > 0) &&
            (component_sum_v<Args...> == static_cast<size_t>(Elements)) &&
            ((is_scalar_or_vec_v<Args>) && ...) &&
            (all_convertible_to_v<T, Args...>)
            >>
            constexpr explicit Mat(const Args&... args) 
        {
            size_t i = 0;
            append_all(i, m, args...);
        }

        constexpr       T& operator()(int r, int c)       noexcept { return m[index(r, c)]; }
        constexpr const T& operator()(int r, int c) const noexcept { return m[index(r, c)]; }

        constexpr RowProxy      operator[](int r)       noexcept { return RowProxy(&m[index(r, 0)]); }
        constexpr ConstRowProxy operator[](int r) const noexcept { return ConstRowProxy(&m[index(r, 0)]); }

        constexpr       T* data()       noexcept { return m; }
        constexpr const  T* data() const noexcept { return m; }
        constexpr       T* begin()       noexcept { return m; }
        constexpr const  T* begin() const noexcept { return m; }
        constexpr       T* end()         noexcept { return m + Elements; }
        constexpr const  T* end()   const noexcept { return m + Elements; }

        // basic helpers
        static constexpr Mat zero() noexcept {
            Mat z{};
            for (int i = 0; i < Elements; ++i) z.m[i] = T(0);
            return z;
        }

        static constexpr Mat identity() noexcept {
            static_assert(R == C, "identity() requires square matrix");
            Mat I = zero();
            for (int i = 0; i < R; ++i) I(i, i) = T(1);
            return I;
        }

        // From rows
        template<class... RowVecs,
            class = std::enable_if_t<(sizeof...(RowVecs) == R) && (is_vec_v<RowVecs> && ...)>>
            static constexpr Mat from_rows(const RowVecs&... rows) 
        {
            Mat M{};
            const Vec<T, C> rr[] = { Vec<T,C>(rows)... };
            for (int r = 0; r < R; ++r) for (int c = 0; c < C; ++c) M(r, c) = rr[r][c];
            return M;
        }

        // From cols.
        template<class... ColVecs,
            class = std::enable_if_t<(sizeof...(ColVecs) == C) && (is_vec_v<ColVecs> && ...)>>
            static constexpr Mat from_cols(const ColVecs&... cols) 
        {
            Mat M{};
            const Vec<T, R> cc[] = { Vec<T,R>(cols)... };
            for (int c = 0; c < C; ++c) for (int r = 0; r < R; ++r) M(r, c) = cc[c][r];
            return M;
        }

        // comparison
        constexpr bool operator==(const Mat& o) const noexcept 
        {
            for (int i = 0; i < Elements; ++i) if (m[i] != o.m[i]) return false;
            return true;
        }
        constexpr bool operator!=(const Mat& o) const noexcept { return !(*this == o); }

        // arithmetic
        constexpr Mat& operator+=(const Mat& b) noexcept { for (int i = 0; i < Elements; ++i) m[i] += b.m[i]; return *this; }
        constexpr Mat& operator-=(const Mat& b) noexcept { for (int i = 0; i < Elements; ++i) m[i] -= b.m[i]; return *this; }
        constexpr Mat& operator*=(T s)          noexcept { for (int i = 0; i < Elements; ++i) m[i] *= s;     return *this; }
        constexpr Mat& operator/=(T s)          noexcept { for (int i = 0; i < Elements; ++i) m[i] /= s;     return *this; }

        friend constexpr Mat operator+(Mat a, const Mat& b) noexcept { a += b; return a; }
        friend constexpr Mat operator-(Mat a, const Mat& b) noexcept { a -= b; return a; }
        friend constexpr Mat operator*(Mat a, T s)          noexcept { a *= s; return a; }
        friend constexpr Mat operator*(T s, Mat a)          noexcept { a *= s; return a; }
        friend constexpr Mat operator/(Mat a, T s)          noexcept { a /= s; return a; }

        template<int K>
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

        // Matrix * Vec (R x C) * Vec<C> = Vec<R>
        friend constexpr Vec<T, R> operator*(const Mat& A, const Vec<T, C>& x) noexcept 
        {
            Vec<T, R> out{};
            for (int r = 0; r < R; ++r) 
            {
                T acc = T(0);
                for (int c = 0; c < C; ++c) acc += A(r, c) * x[c];
                out[r] = acc;
            }
            return out;
        }

        //Vec * Matrix  Vec<R>^T * (R x C) = Vec<C>
        friend constexpr Vec<T, C> operator*(const Vec<T, R>& x, const Mat& A) noexcept 
        {
            Vec<T, C> out{};
            for (int c = 0; c < C; ++c) 
            {
                T acc = T(0);
                for (int r = 0; r < R; ++r) acc += x[r] * A(r, c);
                out[c] = acc;
            }
            return out;
        }

        // Transpose
        constexpr Mat<T, C, R> transpose() const noexcept 
        {
            Mat<T, C, R> t{};
            for (int r = 0; r < R; ++r) for (int c = 0; c < C; ++c) t(c, r) = (*this)(r, c);
            return t;
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