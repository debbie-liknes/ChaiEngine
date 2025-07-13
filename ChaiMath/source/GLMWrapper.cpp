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

namespace chai
{
	/////////////////////////////////////////// Vector Implementation /////////////////////////////////////////
	template<typename T, int N>
	struct VecImpl
	{
		glm::vec<N, T> glmVec{};

		VecImpl() = default;

		template<typename... Args>
		VecImpl(Args... args) : glmVec{ static_cast<T>(args)... } {}

		VecImpl(std::initializer_list<T> init)
		{
			std::copy_n(init.begin(), std::min(static_cast<size_t>(N), init.size()), &glmVec[0]);
		}
	};

	// vec 2 Specialization
	template<typename T>
	Vec<T, 2>::Vec() : impl_(std::make_unique<VecImpl<T, 2>>()),
		x(impl_->glmVec[0]), y(impl_->glmVec[1]) 
	{}

	template<typename T>
	Vec<T, 2>::Vec(const T& xVal, const T& yVal) : impl_(std::make_unique<VecImpl<T, 2>>(xVal, yVal)),
		x(impl_->glmVec[0]), y(impl_->glmVec[1]) 
	{}

	template<typename T>
	Vec<T, 2>::~Vec() = default;

	template<typename T>
	Vec<T, 2>::Vec(const Vec& other) : impl_(std::make_unique<VecImpl<T, 2>>(*other.impl_)),
		x(impl_->glmVec[0]), y(impl_->glmVec[1]) 
	{}

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
		x(impl_->glmVec[0]), y(impl_->glmVec[1]) 
	{}

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
		x(impl_->glmVec[0]), y(impl_->glmVec[1]) 
	{}

	template<typename T>
	Vec<T, 2>::Vec(std::initializer_list<T> init) : impl_(std::make_unique<VecImpl<T, 2>>(init)),
		x(impl_->glmVec[0]), y(impl_->glmVec[1]) 
	{}

	template<typename T>
	T& Vec<T, 2>::operator[](int i)
	{
		return impl_->glmVec[i];
	}

	template<typename T>
	const T& Vec<T, 2>::operator[](int i) const
	{
		return impl_->glmVec[i];
	}

	template<typename T>
	bool Vec<T, 2>::operator==(const Vec& other) const
	{
		return impl_->glmVec[0] == other.impl_->glmVec[0] && impl_->glmVec[1] == other.impl_->glmVec[1];
	}

	//Vec3 Specialization
	template<typename T>
	Vec<T, 3>::Vec() : impl_(std::make_unique<VecImpl<T, 3>>()),
		x(impl_->glmVec[0]), y(impl_->glmVec[1]), z(impl_->glmVec[2]) 
	{}

	template<typename T>
	Vec<T, 3>::Vec(const T& xVal, const T& yVal, const T& zVal) :
		impl_(std::make_unique<VecImpl<T, 3>>(xVal, yVal, zVal)),
		x(impl_->glmVec[0]), y(impl_->glmVec[1]), z(impl_->glmVec[2])
	{}

	template<typename T>
	Vec<T, 3>::~Vec() = default;

	template<typename T>
	Vec<T, 3>::Vec(const Vec& other) : impl_(std::make_unique<VecImpl<T, 3>>(*other.impl_)),
		x(impl_->glmVec[0]), y(impl_->glmVec[1]), z(impl_->glmVec[2]) 
	{}

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
		x(impl_->glmVec[0]), y(impl_->glmVec[1]), z(impl_->glmVec[2]) 
	{}

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
		x(impl_->glmVec[0]), y(impl_->glmVec[1]), z(impl_->glmVec[2]) 
	{}

	template<typename T>
	Vec<T, 3>::Vec(std::initializer_list<T> init) : impl_(std::make_unique<VecImpl<T, 3>>(init)),
		x(impl_->glmVec[0]), y(impl_->glmVec[1]), z(impl_->glmVec[2]) 
	{}

	template<typename T>
	T& Vec<T, 3>::operator[](int i)
	{
		return impl_->glmVec[i];
	}

	template<typename T>
	const T& Vec<T, 3>::operator[](int i) const
	{
		return impl_->glmVec[i];
	}

	template<typename T>
	bool Vec<T, 3>::operator==(const Vec& other) const
	{
		return impl_->glmVec[0] == other.impl_->glmVec[0] &&
			impl_->glmVec[1] == other.impl_->glmVec[1] &&
			impl_->glmVec[2] == other.impl_->glmVec[2];
	}

	template<typename T>
	Vec2T<T> Vec<T, 3>::xy() const
	{
		return { x, y };
	}

	// Vec4 specialization
	template<typename T>
	Vec<T, 4>::Vec() : impl_(std::make_unique<VecImpl<T, 4>>()),
		x(impl_->glmVec[0]), y(impl_->glmVec[1]), z(impl_->glmVec[2]), w(impl_->glmVec[3]) 
	{}

	template<typename T>
	Vec<T, 4>::Vec(const T& xVal, const T& yVal, const T& zVal, const T& wVal) :
		impl_(std::make_unique<VecImpl<T, 4>>(xVal, yVal, zVal, wVal)),
		x(impl_->glmVec[0]), y(impl_->glmVec[1]), z(impl_->glmVec[2]), w(impl_->glmVec[3])
	{}

	template<typename T>
	Vec<T, 4>::~Vec() = default;

	template<typename T>
	Vec<T, 4>::Vec(const Vec& other) : impl_(std::make_unique<VecImpl<T, 4>>(*other.impl_)),
		x(impl_->glmVec[0]), y(impl_->glmVec[1]), z(impl_->glmVec[2]), w(impl_->glmVec[3]) 
	{}

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
		x(impl_->glmVec[0]), y(impl_->glmVec[1]), z(impl_->glmVec[2]), w(impl_->glmVec[3]) 
	{}

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
		x(impl_->glmVec[0]), y(impl_->glmVec[1]), z(impl_->glmVec[2]), w(impl_->glmVec[3]) 
	{}

	template<typename T>
	Vec<T, 4>::Vec(std::initializer_list<T> init) : impl_(std::make_unique<VecImpl<T, 4>>(init)),
		x(impl_->glmVec[0]), y(impl_->glmVec[1]), z(impl_->glmVec[2]), w(impl_->glmVec[3]) 
	{}

	template<typename T>
	T& Vec<T, 4>::operator[](int i)
	{
		return impl_->glmVec[i];
	}

	template<typename T>
	const T& Vec<T, 4>::operator[](int i) const
	{
		return impl_->glmVec[i];
	}

	template<typename T>
	bool Vec<T, 4>::operator==(const Vec& other) const
	{
		return impl_->glmVec[0] == other.impl_->glmVec[0] &&
			impl_->glmVec[1] == other.impl_->glmVec[1] &&
			impl_->glmVec[2] == other.impl_->glmVec[2] &&
			impl_->glmVec[3] == other.impl_->glmVec[3];
	}

	template<typename T>
	Vec3T<T> Vec<T, 4>::xyz() const
	{
		return { x, y, z };
	}

	template<typename T>
	Vec<T, 4>::Vec(const Vec3T<T>& v3, T w)
		: impl_(std::make_unique<VecImpl<T, 4>>(v3[0], v3[1], v3[2], w)),
		x(impl_->glmVec[0]), y(impl_->glmVec[1]), z(impl_->glmVec[2]), w(impl_->glmVec[3])
	{}

	// Vec operators

	template<typename T, int N>
	Vec<T, N> operator+(const Vec<T, N>& a, const Vec<T, N>& b)
	{
		Vec<T, N> result;
		result.impl_->glmVec = a.impl_->glmVec + b.impl_->glmVec;
		return result;
	}

	template<typename T, int N>
	Vec<T, N> operator-(const Vec<T, N>& a, const Vec<T, N>& b)
	{
		Vec<T, N> result;
		result.impl_->glmVec = a.impl_->glmVec - b.impl_->glmVec;
		return result;
	}

	template<typename T, int N>
	Vec<T, N> operator-(const Vec<T, N>& v)
	{
		Vec<T, N> result;
		result.impl_->glmVec = -v.impl_->glmVec;
		return result;
	}


	//////////////////////////////////////////// Matrix Implementation /////////////////////////////////////////

	template<typename T, int C, int R>
	class MatColumnRef<T, C, R>::Impl 
	{
	public:
		Impl(void* ptr)
			: colRef(*reinterpret_cast<glm::vec<R, T>*>(ptr)) 
		{}

		glm::vec<R, T>& colRef;
	};

	template<typename T, int C, int R>
	MatColumnRef<T, C, R>::MatColumnRef(void* glmVecPtr)
		: impl_(std::make_unique<Impl>(glmVecPtr)) 
	{}

	template<typename T, int C, int R>
	MatColumnRef<T, C, R>::~MatColumnRef() = default;

	template<typename T, int C, int R>
	MatColumnRef<T, C, R>::MatColumnRef(const MatColumnRef& other)
		: impl_(std::make_unique<Impl>(*other.impl_)) 
	{}

	template<typename T, int C, int R>
	MatColumnRef<T, C, R>::MatColumnRef(MatColumnRef&& other) noexcept = default;

	template<typename T, int C, int R>
	MatColumnRef<T, C, R>& MatColumnRef<T, C, R>::operator=(const MatColumnRef& other) 
	{
		if (this != &other) 
		{
			impl_ = std::make_unique<Impl>(*other.impl_);
		}
		return *this;
	}

	template<typename T, int C, int R>
	MatColumnRef<T, C, R>& MatColumnRef<T, C, R>::operator=(MatColumnRef&& other) noexcept = default;

	template<typename T, int C, int R>
	MatColumnRef<T, C, R>::operator Vec<T, R>() const 
	{
		Vec<T, R> vec;
		vec.impl_->glmVec = impl_->colRef;
		return vec;
	}

	template<typename T, int C, int R>
	MatColumnRef<T, C, R>& MatColumnRef<T, C, R>::operator=(const Vec<T, R>& v) 
	{
		impl_->colRef = v.impl_->glmVec;
		return *this;
	}

	// MatImpl implementation using GLM matrices
	template<typename T, int C, int R>
	struct MatImpl
	{
		glm::mat<C, R, T> glmMat;

		MatImpl() : glmMat(T(0))
		{
		}

		template<typename... Args>
		MatImpl(Args... args) : glmMat{ static_cast<T>(args)... }
		{
		}

		MatImpl(std::initializer_list<T> init)
		{
			std::copy_n(init.begin(), std::min(static_cast<size_t>(C * R), init.size()), &glmMat[0][0]);
		}

		MatImpl(const MatImpl& other) : glmMat(other.glmMat)
		{
		}

		MatImpl& operator=(const MatImpl& other)
		{
			if (this != &other)
			{
				glmMat = other.glmMat;
			}
			return *this;
		}

		bool operator==(const MatImpl& other) const
		{
			// GLM provides comparison operator
			return glmMat == other.glmMat;
		}
	};

	template<typename T, int C, int R>
	MatColumnRef<T, C, R> Mat<T, C, R>::operator[](int i)
	{
		return MatColumnRef<T, C, R>(&impl_->glmMat[i]);
	}

	template<typename T, int C, int R>
	T MatColumnRef<T, C, R>::operator[](int i) const 
	{
		return impl_->colRef[i];
	}

	template<typename T, int C, int R>
	T& MatColumnRef<T, C, R>::operator[](int i) 
	{
		return impl_->colRef[i];
	}

	template<typename T, int C, int R>
	Mat<T, C, R> Mat<T, C, R>::identity()
	{
		Mat<T, C, R> result;
		result.impl_->glmMat = glm::identity<glm::mat<C, R, T>>();
		return result;
	}

	template<typename T, int C, int R>
	const Vec<T, R> Mat<T, C, R>::operator[](int i) const
	{
		Vec<T, R> vec;
		for (int j = 0; j < R; ++j)
			vec[j] = impl_->glmMat[i][j];

		return vec;
	}

	template<typename T, int C, int R>
	Mat<T, C, R>::Mat() : impl_(std::make_unique<MatImpl<T, C, R>>())
	{
	}

	template<typename T, int C, int R>
	Mat<T, C, R>::~Mat() = default;

	template<typename T, int C, int R>
	Mat<T, C, R>::Mat(const Mat& other) : impl_(std::make_unique<MatImpl<T, C, R>>(*other.impl_))
	{
	}

	template<typename T, int C, int R>
	Mat<T, C, R>& Mat<T, C, R>::operator=(const Mat& other)
	{
		if (this != &other)
		{
			*impl_ = *other.impl_;
		}
		return *this;
	}

	template<typename T, int C, int R>
	Mat<T, C, R>::Mat(Mat&& other) noexcept : impl_(std::move(other.impl_))
	{
	}

	template<typename T, int C, int R>
	Mat<T, C, R>& Mat<T, C, R>::operator=(Mat&& other) noexcept
	{
		if (this != &other)
		{
			impl_ = std::move(other.impl_);
		}
		return *this;
	}

	template<typename T, int C, int R>
	template<typename... Args>
	Mat<T, C, R>::Mat(Args... args) : impl_(std::make_unique<MatImpl<T, C, R>>(args...))
	{
	}

	template<typename T, int C, int R>
	Mat<T, C, R>::Mat(std::initializer_list<T> init) : impl_(std::make_unique<MatImpl<T, C, R>>(init))
	{
	}

	template<typename T, int C, int R>
	bool Mat<T, C, R>::operator==(const Mat& other) const
	{
		return *impl_ == *other.impl_;
	}

	template<typename T, int C, int R>
	Mat<T, C, R> operator*(const Mat<T, C, R>& a, const Mat<T, C, R>& b)
	{
		//return a.impl_->matrix * b.impl_->matrix;
		Mat<T, C, R> result;
		return result;
	}

	// Mat4 specialization
	template<typename T>
	Mat<T, 4, 4>::Mat() : impl_(std::make_unique<MatImpl<T, 4, 4>>())
	{}

	template<typename T>
	Mat<T, 4, 4>::~Mat() = default;

	template<typename T>
	Mat<T, 4, 4>::Mat(const Mat& other) : impl_(std::make_unique<MatImpl<T, 4, 4>>(*other.impl_)) 
	{}

	template<typename T>
	Mat<T, 4, 4>& Mat<T, 4, 4>::operator=(const Mat& other)
	{
		if (this != &other)
		{
			*impl_ = *other.impl_;
		}
		return *this;
	}

	template<typename T>
	Mat<T, 4, 4>::Mat(Mat&& other) noexcept : impl_(std::move(other.impl_)) 
	{}

	template<typename T>
	Mat<T, 4, 4>& Mat<T, 4, 4>::operator=(Mat&& other) noexcept
	{
		if (this != &other)
		{
			impl_ = std::move(other.impl_);
		}
		return *this;
	}

	template<typename T>
	template<typename... Args>
	Mat<T, 4, 4>::Mat(Args... args) : impl_(std::make_unique<VecImpl<T, 4, 4>>(args...)) 
	{}

	template<typename T>
	Mat<T, 4, 4>::Mat(std::initializer_list<T> init) : impl_(std::make_unique<MatImpl<T, 4, 4>>(init)) 
	{}

	// Component access
	template<typename T>
	MatColumnRef<T, 4, 4> Mat<T, 4, 4>::operator[](int i)
	{
		return MatColumnRef<T, 4, 4>(&impl_->glmMat[i]);
	}

	template<typename T>
	const Vec<T, 4> Mat<T, 4, 4>::operator[](int i) const
	{
		Vec<T, 4> vec;
		for (int j = 0; j < 4; ++j)
			vec[j] = impl_->glmMat[i][j];

		return vec;
	}

	template<typename T>
	bool Mat<T, 4, 4>::operator==(const Mat& other) const
	{
		return *impl_ == *other.impl_;
	}

	template<typename T>
	Mat<T, 4, 4> Mat<T, 4, 4>::identity()
	{
		Mat<T, 4, 4> result;
		for (int i = 0; i < 4; ++i)
			result[i][i] = static_cast<T>(1);
		return result;
	}

#if 1
	/////////////////////////////////////////// Quaternion Implementation /////////////////////////////////////////

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

	//////////////////////////////////////// Math Operations Implementation /////////////////////////////////////////
	template<typename T, int N>
	Vec<T, N> normalize(const Vec<T, N>& vec)
	{
		Vec<T, N> result;
		///*result.impl_->glmVec =*/ glm::normalize(vec.impl_->glmVec);
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
		//result.impl_->data = glm::cross(a.impl_->data, b.impl_->data);
		return result;
	}

	template<typename T>
	Mat<T, 4, 4> lookAt(const Vec<T, 3>& eye, const Vec<T, 3>& center, const Vec<T, 3>& up)
	{
		Mat<T, 4, 4> result;
		//result.impl_->data = glm::lookAt(eye.impl_->data, center.impl_->data, up.impl_->data);
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
		Quaternion<T> result;
		//result.impl_->data = glm::inverse(quat.impl_->data);
		return result;
	}

	template<typename T, int C, int R>
	Mat<T, C, R> translate(const Mat<T, C, R>& mat, const Vec<T, 3>& vec)
	{
		Mat<T, C, R> result;
		//result.impl_->data = glm::translate(mat.impl_->data, vec.impl_->data);
		return result;
	}

	template<typename T, int C, int R>
	Mat<T, C, R> scale(const Mat<T, C, R>& mat, const Vec<T, 3>& vec)
	{
		Mat<T, C, R> result;
		//result.impl_->data = glm::scale(mat.impl_->data, vec.impl_->data);
		return result;
	}

	template<typename T>
	Mat<T, 4, 4> Mat4_cast(const Quaternion<T>& quat)
	{
		Mat<T, 4, 4> result;
		//result.impl_->data = glm::mat4_cast(quat.impl_->data);
		return result;
	}

	template<typename T>
	Quaternion<T> Quat_cast(const Mat<T, 4, 4>& mat)
	{
		Quaternion<T> result;
		//result.impl_->data = glm::quat_cast(mat.impl_->data);
		return result;
	}

	template<typename T>
	Vec<T, 3> operator*(const Quaternion<T> quat, const Vec<T, 3>& v)
	{
		Vec<T, 3> result;
		//result.impl_->data = quat.impl_.data * v.impl_.data;
		return result;
	}

	template<typename T>
	Mat<T, 4, 4> perspective(T fov, T aspect, T near, T far)
	{
		Mat<T, 4, 4> result;
		//result.impl_->data = glm::perspective(glm::radians(fov), aspect, near, far);
		return result;
	}
#endif

	// =========================================== Exports ============================================================
	
	// Class template instantiations for float/double
	template class CHAIMATH_EXPORT Vec<float, 2>;
	template class CHAIMATH_EXPORT Vec<float, 3>;
	template class CHAIMATH_EXPORT Vec<float, 4>;

	template class CHAIMATH_EXPORT Vec<double, 2>;
	template class CHAIMATH_EXPORT Vec<double, 3>;
	template class CHAIMATH_EXPORT Vec<double, 4>;

	template CHAIMATH_EXPORT Vec<float, 4>::Vec(const Vec3T<float>&, float);
	template CHAIMATH_EXPORT Vec<double, 4>::Vec(const Vec3T<double>&, double);

	// Operator overloads
	template CHAIMATH_EXPORT Vec<float, 2> operator+(const Vec<float, 2>&, const Vec<float, 2>&);
	template CHAIMATH_EXPORT Vec<float, 3> operator+(const Vec<float, 3>&, const Vec<float, 3>&);
	template CHAIMATH_EXPORT Vec<float, 4> operator+(const Vec<float, 4>&, const Vec<float, 4>&);
	template CHAIMATH_EXPORT Vec<double, 2> operator+(const Vec<double, 2>&, const Vec<double, 2>&);
	template CHAIMATH_EXPORT Vec<double, 3> operator+(const Vec<double, 3>&, const Vec<double, 3>&);
	template CHAIMATH_EXPORT Vec<double, 4> operator+(const Vec<double, 4>&, const Vec<double, 4>&);

	// Subtraction (binary)
	template CHAIMATH_EXPORT Vec<float, 2> operator-(const Vec<float, 2>&, const Vec<float, 2>&);
	template CHAIMATH_EXPORT Vec<float, 3> operator-(const Vec<float, 3>&, const Vec<float, 3>&);
	template CHAIMATH_EXPORT Vec<float, 4> operator-(const Vec<float, 4>&, const Vec<float, 4>&);
	template CHAIMATH_EXPORT Vec<double, 2> operator-(const Vec<double, 2>&, const Vec<double, 2>&);
	template CHAIMATH_EXPORT Vec<double, 3> operator-(const Vec<double, 3>&, const Vec<double, 3>&);
	template CHAIMATH_EXPORT Vec<double, 4> operator-(const Vec<double, 4>&, const Vec<double, 4>&);

	// Unary minus
	template CHAIMATH_EXPORT Vec<float, 2> operator-(const Vec<float, 2>&);
	template CHAIMATH_EXPORT Vec<float, 3> operator-(const Vec<float, 3>&);
	template CHAIMATH_EXPORT Vec<float, 4> operator-(const Vec<float, 4>&);
	template CHAIMATH_EXPORT Vec<double, 2> operator-(const Vec<double, 2>&);
	template CHAIMATH_EXPORT Vec<double, 3> operator-(const Vec<double, 3>&);
	template CHAIMATH_EXPORT Vec<double, 4> operator-(const Vec<double, 4>&);

	template CHAIMATH_EXPORT Vec<float, 2> normalize(const Vec<float, 2>& vec);
	template CHAIMATH_EXPORT Vec<double, 2> normalize(const Vec<double, 2>& vec);
	template CHAIMATH_EXPORT Vec<float, 3> normalize(const Vec<float, 3>& vec);
	template CHAIMATH_EXPORT Vec<double, 3> normalize(const Vec<double, 3>& vec);
	template CHAIMATH_EXPORT Vec<float, 4> normalize(const Vec<float, 4>& vec);
	template CHAIMATH_EXPORT Vec<double, 4> normalize(const Vec<double, 4>& vec);

	template CHAIMATH_EXPORT Vec<float, 3> cross(const Vec<float, 3>& a, const Vec<float, 3>& b);
	template CHAIMATH_EXPORT Vec<double, 3> cross(const Vec<double, 3>& a, const Vec<double, 3>& b);

	template class CHAIMATH_EXPORT MatColumnRef<float, 4, 4>;
	template CHAIMATH_EXPORT MatColumnRef<float, 4, 4>::operator Vec<float, 4>() const;

	// Mat2 instantiations
	template class CHAIMATH_EXPORT Mat<float, 2, 2>;
	template class CHAIMATH_EXPORT Mat<double, 2, 2>;

	// Mat3 instantiations
	template class CHAIMATH_EXPORT Mat<float, 3, 3>;
	template class CHAIMATH_EXPORT Mat<double, 3, 3>;

	// Mat4 instantiations
	template class CHAIMATH_EXPORT Mat<float, 4, 4>;
	template class CHAIMATH_EXPORT Mat<double, 4, 4>;

	template CHAIMATH_EXPORT Mat<float, 2, 2> operator*(const Mat<float, 2, 2>&, const Mat<float, 2, 2>&);
	template CHAIMATH_EXPORT Mat<float, 3, 3> operator*(const Mat<float, 3, 3>&, const Mat<float, 3, 3>&);
	template CHAIMATH_EXPORT Mat<float, 4, 4> operator*(const Mat<float, 4, 4>&, const Mat<float, 4, 4>&);

	template class CHAIMATH_EXPORT Quaternion<float>;
	template class CHAIMATH_EXPORT Quaternion<double>;

	template class CHAIMATH_EXPORT Quaternion<float> operator*(const Quaternion<float>& a, const Quaternion<float>& b);
	template class CHAIMATH_EXPORT Quaternion<double> operator*(const Quaternion<double>& a, const Quaternion<double>& b);

	template CHAIMATH_EXPORT Mat<float, 4, 4> lookAt<float>(const Vec<float, 3>&, const Vec<float, 3>&, const Vec<float, 3>&);
	template CHAIMATH_EXPORT Mat<double, 4, 4> lookAt<double>(const Vec<double, 3>&, const Vec<double, 3>&, const Vec<double, 3>&);

	template CHAIMATH_EXPORT Mat<float, 4, 4> Mat4_cast(const Quaternion<float>& quat);
	template CHAIMATH_EXPORT Mat<double, 4, 4> Mat4_cast(const Quaternion<double>& quat);

	template CHAIMATH_EXPORT Vec<float, 3> operator*(const Quaternion<float> quat, const Vec<float, 3>& v);
	template CHAIMATH_EXPORT Vec<double, 3> operator*(const Quaternion<double> quat, const Vec<double, 3>& v);

	template CHAIMATH_EXPORT Mat<float, 4, 4> perspective(float fov, float aspect, float near, float far);
	template CHAIMATH_EXPORT Mat<double, 4, 4> perspective(double fov, double aspect, double near, double far);

	template CHAIMATH_EXPORT Mat<float, 2, 2> scale(const Mat<float, 2, 2>& mat, const Vec<float, 3>& vec);
	template CHAIMATH_EXPORT Mat<double, 2, 2> scale(const Mat<double, 2, 2>& mat, const Vec<double, 3>& vec);
	template CHAIMATH_EXPORT Mat<float, 3, 3> scale(const Mat<float, 3, 3>& mat, const Vec<float, 3>& vec);
	template CHAIMATH_EXPORT Mat<double, 3, 3> scale(const Mat<double, 3, 3>& mat, const Vec<double, 3>& vec);
	template CHAIMATH_EXPORT Mat<float, 4, 4> scale(const Mat<float, 4, 4>& mat, const Vec<float, 3>& vec);
	template CHAIMATH_EXPORT Mat<double, 4, 4> scale(const Mat<double, 4, 4>& mat, const Vec<double, 3>& vec);

	template CHAIMATH_EXPORT Mat<float, 2, 2>  translate(const Mat<float, 2, 2>& mat, const Vec<float, 3>& vec);
	template CHAIMATH_EXPORT Mat<double, 2, 2> translate(const Mat<double, 2, 2>& mat, const Vec<double, 3>& vec);
	template CHAIMATH_EXPORT Mat<float, 3, 3>  translate(const Mat<float, 3, 3>& mat, const Vec<float, 3>& vec);
	template CHAIMATH_EXPORT Mat<double, 3, 3> translate(const Mat<double, 3, 3>& mat, const Vec<double, 3>& vec);
	template CHAIMATH_EXPORT Mat<float, 4, 4>  translate(const Mat<float, 4, 4>& mat, const Vec<float, 3>& vec);
	template CHAIMATH_EXPORT Mat<double, 4, 4> translate(const Mat<double, 4, 4>& mat, const Vec<double, 3>& vec);

	template Mat<float, 2, 2> Mat<float, 2, 2>::identity();
	template Mat<float, 3, 3> Mat<float, 3, 3>::identity();
	template Mat<float, 4, 4> Mat<float, 4, 4>::identity();

	template Mat<double, 2, 2> Mat<double, 2, 2>::identity();
	template Mat<double, 3, 3> Mat<double, 3, 3>::identity();
	template Mat<double, 4, 4> Mat<double, 4, 4>::identity();

	template CHAIMATH_EXPORT Quaternion<float> inverse(Quaternion<float> quat);
	template CHAIMATH_EXPORT Quaternion<double> inverse(Quaternion<double> quat);

	template CHAIMATH_EXPORT Quaternion<float> Quat_cast(const Mat<float, 4, 4>& mat);
	template CHAIMATH_EXPORT Quaternion<double> Quat_cast(const Mat<double, 4, 4>& mat);
}