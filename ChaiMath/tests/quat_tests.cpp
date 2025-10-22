#include "test_helpers.h"
#include <ChaiMath.h>

namespace chai
{
	TEST(QuatTests, IdentityQuaternion)
	{
		Quat q = Quat::identity();
		EXPECT_FLOAT_EQ(q.x, 0.0f);
		EXPECT_FLOAT_EQ(q.y, 0.0f);
		EXPECT_FLOAT_EQ(q.z, 0.0f);
		EXPECT_FLOAT_EQ(q.w, 1.0f);
	}

	TEST(QuaternionConversionTest, IdentityRoundTrip) 
	{
		Quaternion<float> q = Quaternion<float>::identity();
		Mat4 m = q.toMat4();
		Quaternion<float> q2 = Quat::quatFromMat4(m);

		EXPECT_TRUE(QuatNear(normalize(q), normalize(q2)));
	}

	TEST(QuatTests, QuaternionMultiplication)
	{
		Quat q1(1.0f, 2.0f, 3.0f, 4.0f);
		Quat q2(5.0f, 6.0f, 7.0f, 8.0f);
		Quat result = q1 * q2;
		
		EXPECT_FLOAT_EQ(result.x, 24.0f);   // 4*5 + 1*8 + 2*7 - 3*6 = 24
		EXPECT_FLOAT_EQ(result.y, 48.0f);   // 4*6 - 1*7 + 2*8 + 3*5 = 48
		EXPECT_FLOAT_EQ(result.z, 48.0f);   // 4*7 + 1*6 - 2*5 + 3*8 = 48
		EXPECT_FLOAT_EQ(result.w, -6.0f);   // 4*8 - 1*5 - 2*6 - 3*7 = -6
	}
	 
	TEST(QuatTests, QuaternionNormalization)
	{
		Quat q(1.0f, 2.0f, 3.0f, 4.0f);
		Quat result = normalize(q);
		
		float length = sqrt(result.x * result.x + result.y * result.y + result.z * result.z + result.w * result.w);
		EXPECT_FLOAT_EQ(length, 1.0f);
	}

	TEST(QuaternionConversionTest, RotateVector90Degrees) 
	{
		//// Rotate 90° around Z axis
		//float angle = M_PI / 2.0f;
		//float c = std::cos(angle / 2.0f);
		//float s = std::sin(angle / 2.0f);
		//Quaternion<float> q{ 0.0f, 0.0f, s, c };

		//struct Vec3 { float x, y, z; };
		//Vec3 v{ 1.0f, 0.0f, 0.0f };
		//Vec3 rotated = q.rotate(v); // should become (0, 1, 0)

		//EXPECT_FLOAT_EQ(rotated.x, 0.0f);
		//EXPECT_FLOAT_EQ(rotated.y, 1.0f);
		//EXPECT_FLOAT_EQ(rotated.z, 0.0f);
	}
}