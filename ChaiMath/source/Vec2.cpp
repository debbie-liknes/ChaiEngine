#include <Vec2.h>
#include "internal/glm_internal.h"
#include <stdexcept>

namespace chai
{
	struct Vec2::Impl
	{
		glm::vec2 vec;

		Impl() : vec(0.0f) {}
		Impl(float x, float y) : vec(x, y) {}
		Impl(const glm::vec2& v) : vec(v) {}
	};

	// Special member functions
	Vec2::Vec2() : pImpl(std::make_unique<Impl>()) {}
	Vec2::Vec2(float x, float y) : pImpl(std::make_unique<Impl>(x, y)) {}
	Vec2::Vec2(float value) : pImpl(std::make_unique<Impl>(value, value)) {}
	Vec2::Vec2(const Vec2& other) : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

	Vec2& Vec2::operator=(const Vec2& other) 
	{
		if (this != &other) {
			pImpl = std::make_unique<Impl>(*other.pImpl);
		}
		return *this;
	}

	Vec2::~Vec2() = default;

	// Index operators
	float& Vec2::operator[](size_t index) 
	{
		if (index >= 2) 
		{
			throw std::out_of_range("Vec2 index out of range");
		}
		return (&pImpl->vec.x)[index];
	}

	const float& Vec2::operator[](size_t index) const 
	{
		if (index >= 2) 
		{
			throw std::out_of_range("Vec2 index out of range");
		}
		return (&pImpl->vec.x)[index];
	}

	// Named accessors - getters
	float Vec2::getX() const 
	{
		return pImpl->vec.x;
	}

	float Vec2::getY() const 
	{
		return pImpl->vec.y;
	}

	// Named accessors - setters
	void Vec2::setX(float x) 
	{
		pImpl->vec.x = x;
	}

	void Vec2::setY(float y) 
	{
		pImpl->vec.y = y;
	}

	// Direct reference accessors
	float& Vec2::x() 
	{
		return pImpl->vec.x;
	}

	float& Vec2::y() 
	{
		return pImpl->vec.y;
	}

	const float& Vec2::x() const 
	{
		return pImpl->vec.x;
	}

	const float& Vec2::y() const 
	{
		return pImpl->vec.y;
	}

	// VectorBase template specializations
	//template<>
	//Vec2 VectorBase<Vec2, float, 2>::operator+(const Vec2& other) const {
	//	return Vec2(derived().pImpl->vec.x + other.pImpl->vec.x,
	//		derived().pImpl->vec.y + other.pImpl->vec.y);
	//}

	//template<>
	//Vec2 VectorBase<Vec2, float, 2>::operator-(const Vec2& other) const {
	//	return Vec2(derived().pImpl->vec.x - other.pImpl->vec.x,
	//		derived().pImpl->vec.y - other.pImpl->vec.y);
	//}

	//template<>
	//Vec2 VectorBase<Vec2, float, 2>::operator*(float scalar) const {
	//	return Vec2(derived().pImpl->vec.x * scalar,
	//		derived().pImpl->vec.y * scalar);
	//}

	//template<>
	//Vec2 VectorBase<Vec2, float, 2>::operator/(float scalar) const {
	//	if (scalar == 0.0f) {
	//		throw std::invalid_argument("Division by zero");
	//	}
	//	return Vec2(derived().pImpl->vec.x / scalar,
	//		derived().pImpl->vec.y / scalar);
	//}

	//template<>
	//Vec2 VectorBase<Vec2, float, 2>::operator-() const {
	//	return Vec2(-derived().pImpl->vec.x, -derived().pImpl->vec.y);
	//}

	//template<>
	//Vec2& VectorBase<Vec2, float, 2>::operator+=(const Vec2& other) {
	//	derived().pImpl->vec += other.pImpl->vec;
	//	return derived();
	//}

	//template<>
	//Vec2& VectorBase<Vec2, float, 2>::operator-=(const Vec2& other) {
	//	derived().pImpl->vec -= other.pImpl->vec;
	//	return derived();
	//}

	//template<>
	//Vec2& VectorBase<Vec2, float, 2>::operator*=(float scalar) {
	//	derived().pImpl->vec *= scalar;
	//	return derived();
	//}

	//template<>
	//Vec2& VectorBase<Vec2, float, 2>::operator/=(float scalar) {
	//	if (scalar == 0.0f) {
	//		throw std::invalid_argument("Division by zero");
	//	}
	//	derived().pImpl->vec /= scalar;
	//	return derived();
	//}

	//template<>
	//bool VectorBase<Vec2, float, 2>::operator==(const Vec2& other) const {
	//	return derived().pImpl->vec == other.pImpl->vec;
	//}

	//template<>
	//bool VectorBase<Vec2, float, 2>::operator!=(const Vec2& other) const {
	//	return derived().pImpl->vec != other.pImpl->vec;
	//}

	//template<>
	//float VectorBase<Vec2, float, 2>::dot(const Vec2& other) const {
	//	return glm::dot(derived().pImpl->vec, other.pImpl->vec);
	//}

	//template<>
	//Vec2 VectorBase<Vec2, float, 2>::normalize() const {
	//	glm::vec2 normalized = glm::normalize(derived().pImpl->vec);
	//	return Vec2(normalized.x, normalized.y);
	//}

}