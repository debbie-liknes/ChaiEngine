#include <ChaiMathOps.h>
#include "internal/glm_internal.h"

namespace chai
{
	template<typename T, int N>
	T length(const Vec<T, N>& vec)
	{
		return std::sqrt(dot(vec, vec));
	}

	template<typename T>
	T radians(T degrees)
	{
		return glm::radians(degrees);
	}

	template<typename T, int N>
	Vec<T, N> normalize(const Vec<T, N>& v)
	{
		auto lenSq = dot(v, v);
		if (lenSq == 0)
			return Vec<T, N>();

		T len = std::sqrt(lenSq);
		return v / len;
	}

	//template<typename T>
	//Quaternion<T> normalize(const Quaternion<T>& q)
	//{
	//	T len = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
	//	if (len == T(0)) return Quaternion<T>::identity(); // or throw/assert
	//	return { q.x / len, q.y / len, q.z / len, q.w / len };
	//}

	template<typename T, int N>
	T dot(const Vec<T, N>& a, const Vec<T, N>& b)
	{
		T result = {};
		for (size_t i = 0; i < N; ++i)
		{
			result += a[i] * b[i];
		}
		return result;
	}

	template<typename T>
	Vec3T<T> cross(const Vec3T<T>& a, const Vec3T<T>& b)
	{
		return 
		{
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}

	template<typename T>
	Mat4T<T> translate(const Mat4T<T>& m, const Vec3T<T>& offset)
	{
		Mat4T<T> result = m;
		result[3][0] += offset.x;
		result[3][1] += offset.y;
		result[3][2] += offset.z;
		return result;
	}

	template<typename T>
	Mat4T<T> scale(const Mat4T<T>& m, const Vec3T<T>& offset)
	{
		Mat4T<T> result = m;
		result[0][0] *= offset.x;
		result[1][1] *= offset.y;
		result[2][2] *= offset.z;
		return result;
	}

	template<typename T>
	Vec<T, 3> operator*(const Quaternion<T> quat, const Vec<T, 3>& v)
	{
		Vec<T, 3> uv = cross(quat.vec, v);
		Vec<T, 3> uuv = cross(quat.vec, uv);
		return v + (uv * (2 * quat.w)) + (uuv * 2);
	}

	template<typename T>
	Mat4T<T> lookAt(const Vec3T<T>& eye, const Vec3T<T>& center, const Vec3T<T>& up)
	{
		Vec3T<T> z = normalize(eye - center);
		Vec3T<T> x = normalize(cross(up, z));
		Vec3T<T> y = cross(z, x);            

		Mat4T<T> result{};

		result[0][0] = x.x;
		result[0][1] = y.x;
		result[0][2] = z.x;
		result[0][3] = 0;

		result[1][0] = x.y;
		result[1][1] = y.y;
		result[1][2] = z.y;
		result[1][3] = 0;

		result[2][0] = x.z;
		result[2][1] = y.z;
		result[2][2] = z.z;
		result[2][3] = 0;

		result[3][0] = -dot(x, eye);
		result[3][1] = -dot(y, eye);
		result[3][2] = -dot(z, eye);
		result[3][3] = 1;

		return result;
	}

	template<typename T>
	Mat4T<T> perspective(T fov, T aspect, T near, T far)
	{
		assert(aspect != T(0));
		assert(far != near);

		T f = static_cast<T>(1) / std::tan(fov / static_cast<T>(2));
		T rangeInv = static_cast<T>(1) / (near - far);

		Mat4T<T> result{};

		result[0][0] = f / aspect;
		result[1][1] = f;
		result[2][2] = (far + near) * rangeInv;
		result[2][3] = static_cast<T>(-1);
		result[3][2] = (static_cast<T>(2) * far * near) * rangeInv;
		result[3][3] = static_cast<T>(0);

		return result;
	}


	template CHAIMATH_EXPORT Mat<float, 4, 4> perspective<float>(float, float, float, float);
	template CHAIMATH_EXPORT Mat<double, 4, 4> perspective<double>(double, double, double, double);

	template CHAIMATH_EXPORT Mat<float, 4, 4> lookAt<float>(const Vec3T<float>&, const Vec3T<float>&, const Vec3T<float>&);
	template CHAIMATH_EXPORT Mat<double, 4, 4> lookAt<double>(const Vec3T<double>&, const Vec3T<double>&, const Vec3T<double>&);

	template CHAIMATH_EXPORT Mat<float, 4, 4> scale<float>(const Mat4T<float>&, const Vec3T<float>&);
	template CHAIMATH_EXPORT Mat<double, 4, 4> scale<double>(const Mat4T<double>&, const Vec3T<double>&);

	template CHAIMATH_EXPORT Mat<float, 4, 4> translate<float>(const Mat4T<float>&, const Vec3T<float>&);
	template CHAIMATH_EXPORT Mat<double, 4, 4> translate<double>(const Mat4T<double>&, const Vec3T<double>&);

	template CHAIMATH_EXPORT Vec3T<float> cross<float>(const Vec3T<float>&, const Vec3T<float>&);
	template CHAIMATH_EXPORT Vec3T<double> cross<double>(const Vec3T<double>&, const Vec3T<double>&);

	template CHAIMATH_EXPORT float radians<float>(float);
	template CHAIMATH_EXPORT double radians<double>(double);

    template CHAIMATH_EXPORT float length<float, 2>(const Vec<float, 2>&);
    template CHAIMATH_EXPORT double length<double, 2>(const Vec<double, 2>&);
	template CHAIMATH_EXPORT float length<float, 3>(const Vec<float, 3>&);
	template CHAIMATH_EXPORT double length<double, 3>(const Vec<double, 3>&);
	template CHAIMATH_EXPORT float length<float, 4>(const Vec<float, 4>&);
	template CHAIMATH_EXPORT double length<double, 4>(const Vec<double, 4>&);
}