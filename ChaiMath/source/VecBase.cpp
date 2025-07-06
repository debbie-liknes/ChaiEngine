#include <VecBase.h>
#include "internal/glm_internal.h"
#include <stdexcept>

namespace chai
{
	template<typename T, int N>
	struct VecImpl
	{
		T data[N];

		VecImpl() { for (int i = 0; i < N; ++i) data[i] = T{}; }

		template<typename... Args>
		explicit VecImpl(Args... args) : data{ static_cast<T>(args)... } {}

		VecImpl(std::initializer_list<T> init) 
		{
			int i = 0;
			for (auto it = init.begin(); it != init.end() && i < N; ++it, ++i) 
			{
				data[i] = *it;
			}
			for (; i < N; ++i) data[i] = T{};
		}
	};

	// vec 2 Specialization
	template<typename T>
	Vec<T, 2>::Vec() : impl_(std::make_unique<VecImpl<T, 2>>()),
		x(impl_->data[0]), y(impl_->data[1]) {}

	template<typename T>
	Vec<T, 2>::~Vec() = default;

	template<typename T>
	Vec<T, 2>::Vec(const Vec& other) : impl_(std::make_unique<VecImpl<T, 2>>(*other.impl_)),
		x(impl_->data[0]), y(impl_->data[1])  {}

	template<typename T>
	Vec<T, 2>& Vec<T, 2>::operator=(const Vec& other) 
	{
		if (this != &other) 
		{
			*impl_ = *other.impl_;
		}
		return *this;
	}

	template<typename T>
	Vec<T, 2>::Vec(Vec&& other) noexcept : impl_(std::move(other.impl_)),
		x(impl_->data[0]), y(impl_->data[1]) {}

	template<typename T>
	Vec<T, 2>& Vec<T, 2>::operator=(Vec&& other) noexcept 
	{
		if (this != &other) 
		{
			impl_ = std::move(other.impl_);
		}
		return *this;
	}

	template<typename T>
	template<typename... Args>
	Vec<T, 2>::Vec(Args... args) : impl_(std::make_unique<VecImpl<T, 2>>(args...)),
		x(impl_->data[0]), y(impl_->data[1]) {}

	template<typename T>
	Vec<T, 2>::Vec(std::initializer_list<T> init) : impl_(std::make_unique<VecImpl<T, 2>>(init)),
		x(impl_->data[0]), y(impl_->data[1]) {}

	template<typename T>
	T& Vec<T, 2>::operator[](int i) 
	{
		if (i < 0 || i >= 2) throw std::out_of_range("Vec2 index out of range");
		return impl_->data[i];
	}

	template<typename T>
	const T& Vec<T, 2>::operator[](int i) const 
	{
		if (i < 0 || i >= 2) throw std::out_of_range("Vec2 index out of range");
		return impl_->data[i];
	}

	template<typename T>
	bool Vec<T, 2>::operator==(const Vec& other) const 
	{
		return impl_->data[0] == other.impl_->data[0] && impl_->data[1] == other.impl_->data[1];
	}

	//Vec3 Specialization
	template<typename T>
	Vec<T, 3>::Vec() : impl_(std::make_unique<VecImpl<T, 3>>()),
		x(impl_->data[0]), y(impl_->data[1]), z(impl_->data[2]) {}

	template<typename T>
	Vec<T, 3>::~Vec() = default;

	template<typename T>
	Vec<T, 3>::Vec(const Vec& other) : impl_(std::make_unique<VecImpl<T, 3>>(*other.impl_)),
		x(impl_->data[0]), y(impl_->data[1]), z(impl_->data[2]) {}

	template<typename T>
	Vec<T, 3>& Vec<T, 3>::operator=(const Vec& other) 
	{
		if (this != &other) 
		{
			*impl_ = *other.impl_;
		}
		return *this;
	}

	template<typename T>
	Vec<T, 3>::Vec(Vec&& other) noexcept : impl_(std::move(other.impl_)),
		x(impl_->data[0]), y(impl_->data[1]), z(impl_->data[2]) {}

	template<typename T>
	Vec<T, 3>& Vec<T, 3>::operator=(Vec&& other) noexcept 
	{
		if (this != &other) 
		{
			impl_ = std::move(other.impl_);
		}
		return *this;
	}

	template<typename T>
	template<typename... Args>
	Vec<T, 3>::Vec(Args... args) : impl_(std::make_unique<VecImpl<T, 3>>(args...)),
		x(impl_->data[0]), y(impl_->data[1]), z(impl_->data[2]) {}

	template<typename T>
	Vec<T, 3>::Vec(std::initializer_list<T> init) : impl_(std::make_unique<VecImpl<T, 3>>(init)),
		x(impl_->data[0]), y(impl_->data[1]), z(impl_->data[2]) {}

	template<typename T>
	T& Vec<T, 3>::operator[](int i) 
	{
		if (i < 0 || i >= 3) throw std::out_of_range("Vec3 index out of range");
		return impl_->data[i];
	}

	template<typename T>
	const T& Vec<T, 3>::operator[](int i) const 
	{
		if (i < 0 || i >= 3) throw std::out_of_range("Vec3 index out of range");
		return impl_->data[i];
	}

	template<typename T>
	bool Vec<T, 3>::operator==(const Vec& other) const 
	{
		return impl_->data[0] == other.impl_->data[0] &&
			impl_->data[1] == other.impl_->data[1] &&
			impl_->data[2] == other.impl_->data[2];
	}

	template<typename T>
	Vec<T, 4>::Vec() : impl_(std::make_unique<VecImpl<T, 4>>()),
		x(impl_->data[0]), y(impl_->data[1]), z(impl_->data[2]), w(impl_->data[3]) {}

	template<typename T>
	Vec<T, 4>::~Vec() = default;

	template<typename T>
	Vec<T, 4>::Vec(const Vec& other) : impl_(std::make_unique<VecImpl<T, 4>>(*other.impl_)),
		x(impl_->data[0]), y(impl_->data[1]), z(impl_->data[2]), w(impl_->data[3]) {}

	template<typename T>
	Vec<T, 4>& Vec<T, 4>::operator=(const Vec& other) 
	{
		if (this != &other) 
		{
			*impl_ = *other.impl_;
		}
		return *this;
	}

	template<typename T>
	Vec<T, 4>::Vec(Vec&& other) noexcept : impl_(std::move(other.impl_)),
		x(impl_->data[0]), y(impl_->data[1]), z(impl_->data[2]), w(impl_->data[3]) {}

	template<typename T>
	Vec<T, 4>& Vec<T, 4>::operator=(Vec&& other) noexcept 
	{
		if (this != &other) 
		{
			impl_ = std::move(other.impl_);
		}
		return *this;
	}

	template<typename T>
	template<typename... Args>
	Vec<T, 4>::Vec(Args... args) : impl_(std::make_unique<VecImpl<T, 4>>(args...)),
		x(impl_->data[0]), y(impl_->data[1]), z(impl_->data[2]), w(impl_->data[3]) {}

	template<typename T>
	Vec<T, 4>::Vec(std::initializer_list<T> init) : impl_(std::make_unique<VecImpl<T, 4>>(init)),
		x(impl_->data[0]), y(impl_->data[1]), z(impl_->data[2]), w(impl_->data[3]) {}

	template<typename T>
	T& Vec<T, 4>::operator[](int i) 
	{
		if (i < 0 || i >= 4) throw std::out_of_range("Vec4 index out of range");
		return impl_->data[i];
	}

	template<typename T>
	const T& Vec<T, 4>::operator[](int i) const 
	{
		if (i < 0 || i >= 4) throw std::out_of_range("Vec4 index out of range");
		return impl_->data[i];
	}

	template<typename T>
	bool Vec<T, 4>::operator==(const Vec& other) const 
	{
		return impl_->data[0] == other.impl_->data[0] &&
			impl_->data[1] == other.impl_->data[1] &&
			impl_->data[2] == other.impl_->data[2] &&
			impl_->data[3] == other.impl_->data[3];
	}

	//template<typename T, int N>
	//Vec<T, N> operator-(const Vec<T, N>& a, const Vec<T, N>& b)
	//{
	//	Vec<T, N> result;
	//	result.impl_->data = a.impl_->data - b.impl_->data;
	//	return result;
	//}

	// Vec2 instantiations
	template class CHAIMATH_EXPORT Vec<float, 2>;
	template class CHAIMATH_EXPORT Vec<double, 2>;

	// Vec3 instantiations
	template class CHAIMATH_EXPORT Vec<float, 3>;
	template class CHAIMATH_EXPORT Vec<double, 3>;

	// Vec4 instantiations
	template class CHAIMATH_EXPORT Vec<float, 4>;
	template class CHAIMATH_EXPORT Vec<double, 4>;

}