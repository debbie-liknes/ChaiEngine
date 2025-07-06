#pragma once


namespace chai
{
    //template<typename T, int N>
    //struct VecImpl;

    //// Traits to determine which components are available
    //template<int N>
    //struct VecTraits
    //{
    //    static constexpr bool has_x = N >= 1;
    //    static constexpr bool has_y = N >= 2;
    //    static constexpr bool has_z = N >= 3;
    //    static constexpr bool has_w = N >= 4;
    //};

    //template<typename T, int N, typename Derived>
    //class VecComponentAccess
    //{
    //protected:
    //    std::unique_ptr<VecImpl<T, N>>& get_impl() { return static_cast<Derived*>(this)->impl_; }
    //    const std::unique_ptr<VecImpl<T, N>>& get_impl() const { return static_cast<const Derived*>(this)->impl_; }
    //};

    //// Specialization for 2D vectors
    //template<typename T, typename Derived>
    //class VecComponentAccess<T, 2, Derived>
    //{
    //protected:
    //    std::unique_ptr<VecImpl<T, 2>>& get_impl() { return static_cast<Derived*>(this)->impl_; }
    //    const std::unique_ptr<VecImpl<T, 2>>& get_impl() const { return static_cast<const Derived*>(this)->impl_; }

    //public:
    //    T& x();
    //    const T& x() const;
    //    T& y();
    //    const T& y() const;
    //};

    //// Specialization for 3D vectors
    //template<typename T, typename Derived>
    //class VecComponentAccess<T, 3, Derived>
    //{
    //protected:
    //    std::unique_ptr<VecImpl<T, 3>>& get_impl() { return static_cast<Derived*>(this)->impl_; }
    //    const std::unique_ptr<VecImpl<T, 3>>& get_impl() const { return static_cast<const Derived*>(this)->impl_; }

    //public:
    //    T& x();
    //    const T& x() const;
    //    T& y();
    //    const T& y() const;
    //    T& z();
    //    const T& z() const;
    //};

    //// Specialization for 4D vectors
    //template<typename T, typename Derived>
    //class VecComponentAccess<T, 4, Derived>
    //{
    //protected:
    //    std::unique_ptr<VecImpl<T, 4>>& get_impl() { return static_cast<Derived*>(this)->impl_; }
    //    const std::unique_ptr<VecImpl<T, 4>>& get_impl() const { return static_cast<const Derived*>(this)->impl_; }

    //public:
    //    T& x();
    //    const T& x() const;
    //    T& y();
    //    const T& y() const;
    //    T& z();
    //    const T& z() const;
    //    T& w();
    //    const T& w() const;
    //};
}