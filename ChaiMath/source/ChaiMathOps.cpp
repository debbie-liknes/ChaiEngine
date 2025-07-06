#include <ChaiMathOps.h>
#include "internal/glm_internal.h"

namespace chai
{
    template<typename T, int N>
    Vec<T, N> normalize(const Vec<T, N>& vec) 
    {
        Vec<T, N> result;
        result.impl_->data = glm::normalize(vec.impl_->data);
        return result;
    }

    template<typename T, int N>
    T length(const Vec<T, N>& vec) 
    {
        return glm::length(vec.impl_->data);
    }

    template<typename T, int N>
    T dot(const Vec<T, N>& a, const Vec<T, N>& b) 
    {
        return glm::dot(a.impl_->data, b.impl_->data);
    }

    template<typename T>
    Vec<T, 3> cross(const Vec<T, 3>& a, const Vec<T, 3>& b) 
    {
        Vec<T, 3> result;
        result.impl_->data = glm::cross(a.impl_->data, b.impl_->data);
        return result;
    }

    template<typename T, int N>
    Mat<T, 4> lookAt(const Vec<T, 3>& eye, const Vec<T, 3>& center, const Vec<T, 3>& up)
    {
		Mat<T, 4> result;
        result.impl_->data = glm::lookAt(eye.impl_->data, center.impl_->data, up.impl_->data);
        return result;
    }

    template<typename T>
    T radians(T degrees)
    {
		return glm::radians(degrees);
    }

    template<typename T>
    T inverse(Quaternion<T> quat)
    {
        Quaternion<T> result;
        result.impl_->data = glm::inverse(quat.impl_->data);
		return result;
    }

    template<typename T, int N>
    Mat<T, N> translate(const Mat<T, N>& mat, const Vec<T, 3>& vec)
    {
        Mat<T, N> result;
        result.impl_->data = glm::translate(mat.impl_->data, vec.impl_->data);
		return result;
    }

    template<typename T, int N>
    Mat<T, N> scale(const Mat<T, N>& mat, const Vec<T, 3>& vec)
    {
        Mat<T, N> result;
		result.impl_->data = glm::scale(mat.impl_->data, vec.impl_->data);
		return result;
    }

    template<typename T>
    Mat<T, 4> Mat4_cast(const Quaternion<T>& quat)
    {
        Mat<T, 4> result;
        result.impl_->data = glm::mat4_cast(quat.impl_->data);
		return result;
    }

    template<typename T>
    Quaternion<T> Quat_cast(const Mat<T, 4>& mat)
    {
		Quaternion<T> result;
		result.impl_->data = glm::quat_cast(mat.impl_->data);
		return result;
    }

    template<typename T, int N>
    Vec<T, 3> operator*(const Quaternion<T> quat, const Vec<T, 3>& v)
    {
        Vec<T, 3> result;
        result.impl_->data = quat.impl_.data * v.impl_.data;
        return result;
    }

    template<typename T>
    Mat<T, 4> perspective(T fov, T aspect, T near, T far)
    {
        Mat<T, 4> result;
        result.impl_->data = glm::perspective(glm::radians(fov), aspect, near, far);
		return result;
    }
}