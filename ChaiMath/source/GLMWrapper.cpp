#include <VecBase.h>
#include <ChaiMatrix.h>
#include <ChaiQuat.h>
#include <ChaiMathOps.h>
#include "internal/glm_internal.h"
#include <stdexcept>
#include <Vec2.h>
#include <Vec3.h>
#include <Vec4.h>
#include <Mat_4x4.h>

#if 0

//////////////////////////////////////////////// Quaternion Access ////////////////////////////////////////////
namespace chai
{
	template<typename T>
	struct QuaternionImpl
	{
		glm::qua<T> quat;

		QuaternionImpl() : quat(T(1), T(0), T(0), T(0))
		{
			// Initialize to identity quaternion (w=1, x=0, y=0, z=0)
		}

		QuaternionImpl(T angle, T x, T y, T z)
			: quat(glm::angleAxis(angle, glm::vec<3, T>(x, y, z)))
		{
		}

		QuaternionImpl(T pitch, T yaw, T roll)
			: quat(glm::vec<3, T>(pitch, yaw, roll))
		{
		}

		QuaternionImpl(const QuaternionImpl& other) : quat(other.quat)
		{
		}

		QuaternionImpl& operator=(const QuaternionImpl& other)
		{
			if (this != &other)
			{
				quat = other.quat;
			}
			return *this;
		}

		T& operator[](int i)
		{
			if (i < 0 || i >= 4)
			{
				throw std::out_of_range("Quaternionernion index out of range");
			}
			// GLM quaternion order: w, x, y, z
			return (&quat.w)[i];
		}

		const T& operator[](int i) const
		{
			if (i < 0 || i >= 4)
			{
				throw std::out_of_range("Quaternionernion index out of range");
			}
			return (&quat.w)[i];
		}

		bool operator==(const QuaternionImpl& other) const
		{
			return quat == other.quat;
		}

		const glm::qua<T>& getGLM() const
		{
			return quat;
		}
		glm::qua<T>& getGLM()
		{
			return quat;
		}
	};

	namespace internal
	{
		template<typename T>
		struct QuatAccess
		{
			static glm::qua<T>& get(Quaternion<T>& v)
			{
				return v.impl_->quat;
			}

			static const glm::qua<T>& get(const Quaternion<T>& v)
			{
				return v.impl_->quat;
			}
		};
	}
}

/////////////////////////////////////////// Quaternion Implementation /////////////////////////////////////////
namespace chai
{
	// Quaternion class implementation
	template<typename T>
	Quaternion<T>::Quaternion() : impl_(std::make_unique<QuaternionImpl<T>>())
	{
	}

	template<typename T>
	Quaternion<T>::~Quaternion() = default;

	template<typename T>
	Quaternion<T>::Quaternion(const Quaternion& other) : impl_(std::make_unique<QuaternionImpl<T>>(*other.impl_))
	{
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::operator=(const Quaternion& other)
	{
		if (this != &other)
		{
			*impl_ = *other.impl_;
		}
		return *this;
	}

	template<typename T>
	Quaternion<T>::Quaternion(Quaternion&& other) noexcept : impl_(std::move(other.impl_))
	{
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::operator=(Quaternion&& other) noexcept
	{
		if (this != &other)
		{
			impl_ = std::move(other.impl_);
		}
		return *this;
	}

	template<typename T>
	Quaternion<T>::Quaternion(T angle, T x, T y, T z) : impl_(std::make_unique<QuaternionImpl<T>>(angle, x, y, z))
	{
	}

	template<typename T>
	Quaternion<T>::Quaternion(T pitch, T yaw, T roll) : impl_(std::make_unique<QuaternionImpl<T>>(pitch, yaw, roll))
	{
	}

	template<typename T>
	T& Quaternion<T>::operator[](int i)
	{
		return (*impl_)[i];
	}

	template<typename T>
	const T& Quaternion<T>::operator[](int i) const
	{
		return (*impl_)[i];
	}

	template<typename T>
	T& Quaternion<T>::w() { return impl_->quat.w; }

	template<typename T>
	const T& Quaternion<T>::w() const { return impl_->quat.w; }

	template<typename T>
	T& Quaternion<T>::x() { return impl_->quat.x; }

	template<typename T>
	const T& Quaternion<T>::x() const { return impl_->quat.x; }

	template<typename T>
	T& Quaternion<T>::y() { return impl_->quat.y; }

	template<typename T>
	const T& Quaternion<T>::y() const { return impl_->quat.y; }

	template<typename T>
	T& Quaternion<T>::z() { return impl_->quat.z; }

	template<typename T>
	const T& Quaternion<T>::z() const { return impl_->quat.z; }

	template<typename T>
	bool Quaternion<T>::operator==(const Quaternion& other) const
	{
		return *impl_ == *other.impl_;
	}

	template<typename T>
	bool Quaternion<T>::operator!=(const Quaternion& other) const
	{
		return !(*this == other);
	}

	// Friend function implementations
	template<typename T>
	Quaternion<T> operator+(const Quaternion<T>& a, const Quaternion<T>& b)
	{
		using Access = internal::QuatAccess<T>;
		Quaternion<T> result;
		Access::get(result) = Access::get(a) + Access::get(b);
		return result;
	}

	template<typename T>
	Quaternion<T> operator-(const Quaternion<T>& a, const Quaternion<T>& b)
	{
		using Access = internal::QuatAccess<T>;
		Quaternion<T> result;
		Access::get(result) = Access::get(a) - Access::get(b);
		return result;
	}

	template<typename T>
	Quaternion<T> operator*(const Quaternion<T>& a, const Quaternion<T>& b)
	{
		using Access = internal::QuatAccess<T>;
		Quaternion<T> result;
		Access::get(result) = Access::get(a) * Access::get(b);
		return result;
	}

	template<typename T>
	Quaternion<T> operator*(const Quaternion<T>& q, T scalar)
	{
		using Access = internal::QuatAccess<T>;
		Quaternion<T> result;
		Access::get(result) = Access::get(q) * scalar;
		return result;
	}

	template<typename T>
	Quaternion<T> operator*(T scalar, const Quaternion<T>& q)
	{
		using Access = internal::QuatAccess<T>;
		Quaternion<T> result;
		Access::get(result) = Access::get(q) * scalar;
		return result;
	}

	template<typename T>
	Vec<T, 4> operator*(const Quaternion<T>& a, const Vec<T, 4>& b)
	{
		using Access = internal::QuatAccess<T>;
		using VecAccess = internal::VecAccess<T, 4>;
		Vec<T, 4> result;
		VecAccess::get(result) = Access::get(a) * VecAccess::get(b);
		return result;
	}

	template<typename T>
	Vec<T, 4> operator*(const Vec<T, 4>& a, const Quaternion<T>& b)
	{
		using Access = internal::QuatAccess<T>;
		using VecAccess = internal::VecAccess<T, 4>;
		Vec<T, 4> result;
		VecAccess::get(result) = VecAccess::get(a) * Access::get(b);
		return result;
	}
}

//////////////////////////////////////// Math Operations Implementation /////////////////////////////////////////
namespace chai
{
	template<typename T, int N>
	Vec<T, N> normalize(const Vec<T, N>& vec)
	{
		using Access = internal::VecAccess<T, N>;
		Vec<T, N> result;
		Access::get(result) = glm::normalize(Access::get(vec));
		return result;
	}

	template<typename T, int N>
	T length(const Vec<T, N>& vec)
	{
		using Access = internal::VecAccess<T, N>;
		return glm::length(Access::get(vec));
	}

	template<typename T, int N>
	T dot(const Vec<T, N>& a, const Vec<T, N>& b)
	{
		using Access = internal::VecAccess<T, N>;
		return glm::dot(Access::get(a), Access::get(b));
	}

	template<typename T>
	Vec<T, 3> cross(const Vec<T, 3>& a, const Vec<T, 3>& b)
	{
		using Access = internal::VecAccess<T, 3>;
		Vec<T, 3> result;
		Access::get(result) = glm::cross(Access::get(a), Access::get(b));
		return result;
	}

	template<typename T>
	Mat<T, 4, 4> lookAt(const Vec<T, 3>& eye, const Vec<T, 3>& center, const Vec<T, 3>& up)
	{
		using Access = internal::MatAccess<T, 4, 4>;
		using VecAccess = internal::VecAccess<T, 3>;
		Mat<T, 4, 4> result;
		Access::get(result) = glm::lookAt(VecAccess::get(eye), VecAccess::get(center), VecAccess::get(up));
		return result;
	}

	template<typename T>
	T radians(T degrees)
	{
		return glm::radians(degrees);
	}

	template<typename T>
	Quaternion<T> inverse(Quaternion<T> quat)
	{
		using Access = internal::QuatAccess<T>;
		Quaternion<T> result;
		Access::get(result) = glm::inverse(Access::get(quat));
		return result;
	}

	template<typename T>
	Mat<T, 4, 4> translate(const Mat<T, 4, 4>& mat, const Vec<T, 3>& vec)
	{
		using Access = internal::MatAccess<T, 4, 4>;
		using VecAccess = internal::VecAccess<T, 3>;
		Mat<T, 4, 4> result;
		Access::get(result) = glm::translate(Access::get(mat), VecAccess::get(vec));
		return result;
	}

	template<typename T>
	Mat<T, 4, 4> scale(const Mat<T, 4, 4>& mat, const Vec<T, 3>& vec)
	{
		using Access = internal::MatAccess<T, 4, 4>;
		using VecAccess = internal::VecAccess<T, 3>;
		Mat<T, 4, 4> result;
		Access::get(result) = glm::scale(Access::get(mat), VecAccess::get(vec));
		return result;
	}

	template<typename T>
	Mat<T, 4, 4> Mat4_cast(const Quaternion<T>& quat)
	{
		using MatAccess = internal::MatAccess<T, 4, 4>;
		using QuatAccess = internal::QuatAccess<T>;
		Mat<T, 4, 4> result;
		MatAccess::get(result) = glm::mat4_cast(QuatAccess::get(quat));
		return result;
	}

	template<typename T>
	Quaternion<T> Quat_cast(const Mat<T, 4, 4>& mat)
	{
		using MatAccess = internal::MatAccess<T, 4, 4>;
		using QuatAccess = internal::QuatAccess<T>;
		Quaternion<T> result;
		QuatAccess::get(result) = glm::quat_cast(MatAccess::get(mat));
		return result;
	}

	template<typename T>
	Vec<T, 3> operator*(const Quaternion<T> quat, const Vec<T, 3>& v)
	{
		using VecAccess = internal::VecAccess<T, 3>;
		using QuatAccess = internal::QuatAccess<T>;
		Vec<T, 3> result;
		VecAccess::get(result) = QuatAccess::get(quat) * VecAccess::get(v);
		return result;
	}

	template<typename T>
	Mat<T, 4, 4> perspective(T fov, T aspect, T near, T far)
	{
		using MatAccess = internal::MatAccess<T, 4, 4>;
		Mat<T, 4, 4> result;
		MatAccess::get(result) = glm::perspective(glm::radians(fov), aspect, near, far);
		return result;
	}
}
#endif

//////////////////////////////////////////////// Exports //////////////////////////////////////////////////////
namespace chai
{
	//template CHAIMATH_EXPORT Vec<double, 2>::Vec(double, double);
	//template CHAIMATH_EXPORT Vec<int, 2>::Vec(int, int);

	//// Class template instantiations for float/double
	//template class CHAIMATH_EXPORT Vec<float, 2>;
	//template class CHAIMATH_EXPORT Vec<double, 2>;
	//template class CHAIMATH_EXPORT Vec<float, 3>;
	//template class CHAIMATH_EXPORT Vec<float, 4>;

	//template class CHAIMATH_EXPORT Vec<double, 3>;
	//template class CHAIMATH_EXPORT Vec<double, 3>;
	//template class CHAIMATH_EXPORT Vec<double, 4>;

	//template CHAIMATH_EXPORT float& Vec<float, 2>::operator[](int);
	//template CHAIMATH_EXPORT const float& Vec<float, 2>::operator[](int) const;

	//template CHAIMATH_EXPORT double& Vec<double, 2>::operator[](int);
	//template CHAIMATH_EXPORT const double& Vec<double, 2>::operator[](int) const;

	//// Operator overloads
	//template CHAIMATH_EXPORT Vec<float, 3> operator+(const Vec<float, 3>&, const Vec<float, 3>&);
	//template CHAIMATH_EXPORT Vec<float, 3> operator+(const Vec<float, 3>&, const Vec<float, 3>&);
	//template CHAIMATH_EXPORT Vec<float, 4> operator+(const Vec<float, 4>&, const Vec<float, 4>&);
	//template CHAIMATH_EXPORT Vec<double, 3> operator+(const Vec<double, 3>&, const Vec<double, 3>&);
	//template CHAIMATH_EXPORT Vec<double, 3> operator+(const Vec<double, 3>&, const Vec<double, 3>&);
	//template CHAIMATH_EXPORT Vec<double, 4> operator+(const Vec<double, 4>&, const Vec<double, 4>&);

	//// Subtraction (binary)
	//template CHAIMATH_EXPORT Vec<float, 3> operator-(const Vec<float, 3>&, const Vec<float, 3>&);
	//template CHAIMATH_EXPORT Vec<float, 3> operator-(const Vec<float, 3>&, const Vec<float, 3>&);
	//template CHAIMATH_EXPORT Vec<float, 4> operator-(const Vec<float, 4>&, const Vec<float, 4>&);
	//template CHAIMATH_EXPORT Vec<double, 3> operator-(const Vec<double, 3>&, const Vec<double, 3>&);
	//template CHAIMATH_EXPORT Vec<double, 3> operator-(const Vec<double, 3>&, const Vec<double, 3>&);
	//template CHAIMATH_EXPORT Vec<double, 4> operator-(const Vec<double, 4>&, const Vec<double, 4>&);

	//// Unary minus
	//template CHAIMATH_EXPORT Vec<float, 3> operator-(const Vec<float, 3>&);
	//template CHAIMATH_EXPORT Vec<float, 3> operator-(const Vec<float, 3>&);
	//template CHAIMATH_EXPORT Vec<float, 4> operator-(const Vec<float, 4>&);
	//template CHAIMATH_EXPORT Vec<double, 3> operator-(const Vec<double, 3>&);
	//template CHAIMATH_EXPORT Vec<double, 3> operator-(const Vec<double, 3>&);
	//template CHAIMATH_EXPORT Vec<double, 4> operator-(const Vec<double, 4>&);

	//template CHAIMATH_EXPORT float& Vec<float, 2>::operator[](int);
	//template CHAIMATH_EXPORT const float& Vec<float, 2>::operator[](int) const;

	//template CHAIMATH_EXPORT double& Vec<double, 2>::operator[](int);
	//template CHAIMATH_EXPORT const double& Vec<double, 2>::operator[](int) const;

	//template CHAIMATH_EXPORT int& Vec<int, 2>::operator[](int);
	//template CHAIMATH_EXPORT const int& Vec<int, 2>::operator[](int) const;

	//template CHAIMATH_EXPORT Vec<float, 3> normalize(const Vec<float, 3>& vec);
	//template CHAIMATH_EXPORT Vec<float, 3> normalize(const Vec<float, 3>& vec);
	//template CHAIMATH_EXPORT Vec<float, 4> normalize(const Vec<float, 4>& vec);
	//template CHAIMATH_EXPORT Vec<double, 3> normalize(const Vec<double, 3>& vec);
	//template CHAIMATH_EXPORT Vec<double, 3> normalize(const Vec<double, 3>& vec);
	//template CHAIMATH_EXPORT Vec<double, 4> normalize(const Vec<double, 4>& vec);

	//template CHAIMATH_EXPORT Vec<float, 3> cross(const Vec<float, 3>&, const Vec<float, 3>&);
	//template CHAIMATH_EXPORT Vec<double, 3> cross(const Vec<double, 3>&, const Vec<double, 3>&);

	//template CHAIMATH_EXPORT Vec<float, 2>::Vec(std::initializer_list<float>);
	//template CHAIMATH_EXPORT Vec<float, 3>::Vec(std::initializer_list<float>);
	//template CHAIMATH_EXPORT Vec<float, 4>::Vec(std::initializer_list<float>);

	//template class CHAIMATH_EXPORT MatColumnRef<float, 4, 4>;
	//template CHAIMATH_EXPORT MatColumnRef<float, 4, 4>::operator Vec<float, 4>() const;

	//// Mat3 instantiations
	//template class CHAIMATH_EXPORT Mat<float, 3, 3>;
	//template class CHAIMATH_EXPORT Mat<double, 3, 3>;

	//// Mat3 instantiations
	//template class CHAIMATH_EXPORT Mat<float, 3, 3>;
	//template class CHAIMATH_EXPORT Mat<double, 3, 3>;

	//// Mat4 instantiations
	//template class CHAIMATH_EXPORT Mat<float, 4, 4>;
	//template class CHAIMATH_EXPORT Mat<double, 4, 4>;

	//template CHAIMATH_EXPORT Mat<float, 3, 3> operator*(const Mat<float, 3, 3>&, const Mat<float, 3, 3>&);
	//template CHAIMATH_EXPORT Mat<float, 3, 3> operator*(const Mat<float, 3, 3>&, const Mat<float, 3, 3>&);
	//template CHAIMATH_EXPORT Mat<float, 4, 4> operator*(const Mat<float, 4, 4>&, const Mat<float, 4, 4>&);

	//template class CHAIMATH_EXPORT Quaternion<float>;
	//template class CHAIMATH_EXPORT Quaternion<double>;

	//template CHAIMATH_EXPORT Quaternion<float> operator*(const Quaternion<float>& a, const Quaternion<float>& b);
	//template CHAIMATH_EXPORT Quaternion<double> operator*(const Quaternion<double>& a, const Quaternion<double>& b);

	//template CHAIMATH_EXPORT Vec<float, 4> operator*(const Quaternion<float>& a, const Vec<float, 4>& b);
	//template CHAIMATH_EXPORT Vec<double, 4> operator*(const Quaternion<double>& a, const Vec<double, 4>& b);

	//template CHAIMATH_EXPORT Vec<float, 4> operator*(const Vec<float, 4>& a, const Quaternion<float>& b);
	//template CHAIMATH_EXPORT Vec<double, 4> operator*(const Vec<double, 4>& a, const Quaternion<double>& b);

	//template CHAIMATH_EXPORT Mat<float, 4, 4> lookAt<float>(const Vec<float, 3>&, const Vec<float, 3>&, const Vec<float, 3>&);
	//template CHAIMATH_EXPORT Mat<double, 4, 4> lookAt<double>(const Vec<double, 3>&, const Vec<double, 3>&, const Vec<double, 3>&);

	//template CHAIMATH_EXPORT Mat<float, 4, 4> Mat4_cast(const Quaternion<float>& quat);
	//template CHAIMATH_EXPORT Mat<double, 4, 4> Mat4_cast(const Quaternion<double>& quat);

	//template CHAIMATH_EXPORT Vec<float, 3> operator*(const Quaternion<float> quat, const Vec<float, 3>& v);
	//template CHAIMATH_EXPORT Vec<double, 3> operator*(const Quaternion<double> quat, const Vec<double, 3>& v);

	//template CHAIMATH_EXPORT Mat<float, 4, 4> perspective(float fov, float aspect, float near, float far);
	//template CHAIMATH_EXPORT Mat<double, 4, 4> perspective(double fov, double aspect, double near, double far);

	//template CHAIMATH_EXPORT Mat<float, 4, 4> scale(const Mat<float, 4, 4>& mat, const Vec<float, 3>& vec);
	//template CHAIMATH_EXPORT Mat<double, 4, 4> scale(const Mat<double, 4, 4>& mat, const Vec<double, 3>& vec);

	//template CHAIMATH_EXPORT Mat<float, 4, 4>  translate(const Mat<float, 4, 4>& mat, const Vec<float, 3>& vec);
	//template CHAIMATH_EXPORT Mat<double, 4, 4> translate(const Mat<double, 4, 4>& mat, const Vec<double, 3>& vec);

	//template Mat<float, 3, 3> Mat<float, 3, 3>::identity();
	//template Mat<float, 3, 3> Mat<float, 3, 3>::identity();
	//template Mat<float, 4, 4> Mat<float, 4, 4>::identity();

	//template Mat<double, 3, 3> Mat<double, 3, 3>::identity();
	//template Mat<double, 3, 3> Mat<double, 3, 3>::identity();
	//template Mat<double, 4, 4> Mat<double, 4, 4>::identity();

	//template CHAIMATH_EXPORT Quaternion<float> inverse(Quaternion<float> quat);
	//template CHAIMATH_EXPORT Quaternion<double> inverse(Quaternion<double> quat);

	//template CHAIMATH_EXPORT Quaternion<float> Quat_cast(const Mat<float, 4, 4>& mat);
	//template CHAIMATH_EXPORT Quaternion<double> Quat_cast(const Mat<double, 4, 4>& mat);

	//template CHAIMATH_EXPORT float radians(float degrees);
	//template CHAIMATH_EXPORT double radians(double degrees);
}