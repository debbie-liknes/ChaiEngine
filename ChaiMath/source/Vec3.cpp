//#include <Vec3.h>
//#include <glm/glm.hpp>
//#include <stdexcept>
//
//namespace chai
//{
//	struct Vec3::Impl
//	{
//		glm::vec3 vec;
//
//		Impl() : vec(0.0f) {}
//		Impl(float x, float y, float z) : vec(x, y, z) {}
//		explicit Impl(const glm::vec3& v) : vec(v) {}
//	};
//
//	// Special member functions
//	Vec3::Vec3() : pImpl(std::make_unique<Impl>()) {}
//	Vec3::Vec3(float x, float y, float z) : pImpl(std::make_unique<Impl>(x, y, z)) {}
//	Vec3::Vec3(float value) : pImpl(std::make_unique<Impl>(value, value, value)) {}
//	Vec3::Vec3(const Vec3& other) : pImpl(std::make_unique<Impl>(*other.pImpl)) {}
//
//	Vec3& Vec3::operator=(const Vec3& other)
//	{
//		if (this != &other) 
//		{
//			pImpl = std::make_unique<Impl>(*other.pImpl);
//		}
//		return *this;
//	}
//
//	Vec3::~Vec3() = default;
//
//	// Index operators
//	float& Vec3::operator[](size_t index)
//	{
//		if (index >= 2)
//		{
//			throw std::out_of_range("Vec3 index out of range");
//		}
//		return (&pImpl->vec.x)[index];
//	}
//
//	const float& Vec3::operator[](size_t index) const
//	{
//		if (index >= 2)
//		{
//			throw std::out_of_range("Vec3 index out of range");
//		}
//		return (&pImpl->vec.x)[index];
//	}
//
//	// Named accessors - getters
//	float Vec3::getX() const
//	{
//		return pImpl->vec.x;
//	}
//
//	float Vec3::getY() const
//	{
//		return pImpl->vec.y;
//	}
//
//	float Vec3::getZ() const
//	{
//		return pImpl->vec.y;
//	}
//
//	// Named accessors - setters
//	void Vec3::setX(float x)
//	{
//		pImpl->vec.x = x;
//	}
//
//	void Vec3::setY(float y)
//	{
//		pImpl->vec.y = y;
//	}
//
//	void Vec3::setZ(float y)
//	{
//		pImpl->vec.y = y;
//	}
//
//	// Direct reference accessors
//	float& Vec3::x()
//	{
//		return pImpl->vec.x;
//	}
//
//	float& Vec3::y()
//	{
//		return pImpl->vec.y;
//	}
//
//	float& Vec3::z()
//	{
//		return pImpl->vec.y;
//	}
//
//	const float& Vec3::x() const
//	{
//		return pImpl->vec.x;
//	}
//
//	const float& Vec3::y() const
//	{
//		return pImpl->vec.y;
//	}
//
//	const float& Vec3::z() const
//	{
//		return pImpl->vec.y;
//	}
//}