#include <ChaiMathOps.h>

#define _USE_MATH_DEFINES
#include <cmath>

namespace chai
{
	//template<typename T>
	//Mat<T, 4, 4> Mat4_cast(const Quaternion<T>& q)
	//{
	//	T x = q.x, y = q.y, z = q.z, w = q.w;
	//	T xx = x * x, yy = y * y, zz = z * z;
	//	T xy = x * y, xz = x * z, yz = y * z;
	//	T wx = w * x, wy = w * y, wz = w * z;

	//	Mat4x4T<T> m{};

	//	m[0][0] = 1 - 2 * (yy + zz);
	//	m[0][1] = 2 * (xy + wz);
	//	m[0][2] = 2 * (xz - wy);
	//	m[0][3] = 0;

	//	m[1][0] = 2 * (xy - wz);
	//	m[1][1] = 1 - 2 * (xx + zz);
	//	m[1][2] = 2 * (yz + wx);
	//	m[1][3] = 0;

	//	m[2][0] = 2 * (xz + wy);
	//	m[2][1] = 2 * (yz - wx);
	//	m[2][2] = 1 - 2 * (xx + yy);
	//	m[2][3] = 0;

	//	m[3][0] = m[3][1] = m[3][2] = 0;
	//	m[3][3] = 1;

	//	return m;
	//}

	//template<typename T>
	//Quaternion<T> Quat_cast(const Mat<T, 4, 4>& m)
	//{
	//	T trace = m[0][0] + m[1][1] + m[2][2];
	//	Quaternion<T> q;

	//	if (trace > 0)
	//	{
	//		T s = std::sqrt(trace + 1.0) * 2;
	//		q.w = 0.25 * s;
	//		q.x = (m[2][1] - m[1][2]) / s;
	//		q.y = (m[0][2] - m[2][0]) / s;
	//		q.z = (m[1][0] - m[0][1]) / s;
	//	}
	//	else
	//	{
	//		if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
	//		{
	//			T s = std::sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2;
	//			q.w = (m[2][1] - m[1][2]) / s;
	//			q.x = 0.25 * s;
	//			q.y = (m[0][1] + m[1][0]) / s;
	//			q.z = (m[0][2] + m[2][0]) / s;
	//		}
	//		else if (m[1][1] > m[2][2])
	//		{
	//			T s = std::sqrt(1.0 + m[1][1] - m[0][0] - m[2][2]) * 2;
	//			q.w = (m[0][2] - m[2][0]) / s;
	//			q.x = (m[0][1] + m[1][0]) / s;
	//			q.y = 0.25 * s;
	//			q.z = (m[1][2] + m[2][1]) / s;
	//		}
	//		else
	//		{
	//			T s = std::sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]) * 2;
	//			q.w = (m[1][0] - m[0][1]) / s;
	//			q.x = (m[0][2] + m[2][0]) / s;
	//			q.y = (m[1][2] + m[2][1]) / s;
	//			q.z = 0.25 * s;
	//		}
	//	}

	//	return normalize(q);
	//}

	//template<typename T>
	//Quaternion<T> operator*(const Quaternion<T>& a, const Quaternion<T>& b)
	//{
	//	return 
	//	{
	//		a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
	//		a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
	//		a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
	//		a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
	//	};
	//}

	//template<typename T>
	//Quaternion<T> inverse(const Quaternion<T>& q)
	//{
	//	T normSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	//	if (normSq == T(0)) return Quaternion<T>::identity(); // or throw

	//	return 
	//	{
	//		-q.x / normSq,
	//		-q.y / normSq,
	//		-q.z / normSq,
	//		 q.w / normSq
	//	};
	//}

	//template<typename T>
	//Vec<T, 4> operator*(const Quaternion<T>& q, const Vec<T, 4>& v)
	//{
	//	// Only rotate the x, y, z components
	//	Vec3T<T> p{ v.x, v.y, v.z };

	//	Quaternion<T> qvec{ p.x, p.y, p.z, 0 };
	//	Quaternion<T> qinv = inverse(q);

	//	Quaternion<T> result = q * qvec * qinv;

	//	return { result.x, result.y, result.z, v.w }; // preserve original w
	//}

	//template<typename T>
	//Vec<T, 4> operator*(const Vec<T, 4>& v, const Quaternion<T>& q)
	//{
	//	// Only rotate the x, y, z components
	//	Vec3T<T> p{ v.x, v.y, v.z };
	//	
	//	Quaternion<T> qvec{ p.x, p.y, p.z, 0 };
	//	Quaternion<T> qinv = inverse(q);

	//	Quaternion<T> result = q * qvec * qinv;

	//	return { result.x, result.y, result.z, v.w }; // preserve original w
	//}

	//template class Quaternion<float>;
	//template class Quaternion<double>;

	//template CHAIMATH_EXPORT Mat<float, 4, 4> Mat4_cast(const Quaternion<float>&);
	//template CHAIMATH_EXPORT Mat<double, 4, 4> Mat4_cast(const Quaternion<double>&);

	//template CHAIMATH_EXPORT Quaternion<float> Quat_cast(const Mat<float, 4, 4>&);
	//template CHAIMATH_EXPORT Quaternion<double> Quat_cast(const Mat<double, 4, 4>&);

	//template CHAIMATH_EXPORT Quaternion<float> operator*(const Quaternion<float>&, const Quaternion<float>&);
	//template CHAIMATH_EXPORT Quaternion<double> operator*(const Quaternion<double>&, const Quaternion<double>&);

	//template CHAIMATH_EXPORT Vec<float, 4> operator*(const Quaternion<float>&, const Vec<float, 4>&);
	//template CHAIMATH_EXPORT Vec<double, 4> operator*(const Quaternion<double>&, const Vec<double, 4>&);
	//template CHAIMATH_EXPORT Vec<float, 4> operator*(const Vec<float, 4>&, const Quaternion<float>&);
	//template CHAIMATH_EXPORT Vec<double, 4> operator*(const Vec<double, 4>&, const Quaternion<double>&);

	//template CHAIMATH_EXPORT Quaternion<float> inverse(const Quaternion<float>&);
	//template CHAIMATH_EXPORT Quaternion<double> inverse(const Quaternion<double>&);
}