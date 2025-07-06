#include <ChaiQuat.h>
#include "internal/glm_internal.h"
#include <stdexcept>

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
		Quaternion<T> result;
		result.impl_->quat = a.impl_->quat + b.impl_->quat;
		return result;
	}

	template<typename T>
	Quaternion<T> operator-(const Quaternion<T>& a, const Quaternion<T>& b)
	{
		Quaternion<T> result;
		result.impl_->quat = a.impl_->quat - b.impl_->quat;
		return result;
	}

	template<typename T>
	Quaternion<T> operator*(const Quaternion<T>& a, const Quaternion<T>& b)
	{
		Quaternion<T> result;
		result.impl_->quat = a.impl_->quat * b.impl_->quat;
		return result;
	}

	template<typename T>
	Quaternion<T> operator*(const Quaternion<T>& q, T scalar)
	{
		Quaternion<T> result;
		result.impl_->quat = q.impl_->quat * scalar;
		return result;
	}

	template<typename T>
	Quaternion<T> operator*(T scalar, const Quaternion<T>& q)
	{
		return q * scalar;
	}
}