#pragma once
#include <ChaiMathExport.h>
#include <memory>
#include <initializer_list>
#include <ChaiMatrix.h>
#include <Vec4.h>

namespace chai
{
 //   template<typename T>
 //   class CHAIMATH_EXPORT Quaternion
 //   {
 //   public:
 //       T x, y, z, w;

 //       constexpr Quaternion() : x(0), y(0), z(0), w(1) {}
 //       constexpr Quaternion(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

 //       static constexpr Quaternion identity() 
 //       {
 //           return { 0, 0, 0, 1 };
 //       }
 //   };

 //   // Type aliases
 //   using Quatf = Quaternion<float>;
 //   using Quatd = Quaternion<double>;
	//using Quat = Quatf;

 //   template<typename T>
 //   Mat<T, 4, 4> Mat4_cast(const Quaternion<T>& quat);

 //   template<typename T>
 //   Quaternion<T> Quat_cast(const Mat<T, 4, 4>& mat);

 //   template<typename T>
 //   Quaternion<T> operator*(const Quaternion<T>& a, const Quaternion<T>& b);

 //   template<typename T>
 //   Quaternion<T> inverse(const Quaternion<T>& a);

 //   //this is dubious
 //   template<typename T>
 //   Vec<T, 4> operator*(const Quaternion<T>& q, const Vec<T, 4>& v);

 //   //dubious pt 2
 //   template<typename T>
 //   Vec<T, 4> operator*(const Vec<T, 4>& q, const Quaternion<T>& v);
}