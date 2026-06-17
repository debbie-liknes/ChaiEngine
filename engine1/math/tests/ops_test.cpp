#include "test_helpers.h"

#include <ChaiMath.h>
#include <ChaiMathOps.h>

namespace math_test
{
    using chai::math::Vec3;
    using chai::math::Vec4;
    using chai::math::Vec3T;
    using chai::math::Vec2;
    using chai::math::Quat;
    using chai::math::Mat4;
    using chai::math::kEpsilonF;

    TEST(OpsTests, Dot)
    {
        Vec3 v(3.0f, 4.0f, 0.0f);
        Vec3 v1(3.0f, 4.0f, 0.0f);

        EXPECT_NEAR(dot(v, v1), 25.0f, kEpsilonF);
    }

    TEST(OpsTests, Length)
    {
        Vec3 v(3.0f, 4.0f, 0.0f);

        EXPECT_NEAR(length(v), 5.0f, kEpsilonF);
    }

    TEST(NormalTests, Normalize)
    {
        Vec3 v{{3.0f, 4.0f, 0.0f}};
        Vec3 n = normalize(v);

        float len = length(n);
        EXPECT_NEAR(len, 1.0f, kEpsilonF);

        EXPECT_NEAR(n[0] / n[1], v[0] / v[1], kEpsilonF);
    }

    TEST(NormalTests, NormZeroVectorReturnsZero)
    {
        Vec3 v{{0.0f, 0.0f, 0.0f}};
        Vec3 n = normalize(v);

        EXPECT_EQ(n[0], 0.0f);
        EXPECT_EQ(n[1], 0.0f);
        EXPECT_EQ(n[2], 0.0f);
    }

    TEST(CrossProductTest, OrthogonalAxes)
    {
        Vec3 x{1, 0, 0};
        Vec3 y{0, 1, 0};
        Vec3 z{0, 0, 1};

        EXPECT_TRUE(VecNear(cross(x, y), z, kEpsilonF));
        EXPECT_TRUE(VecNear(cross(y, z), x, kEpsilonF));
        EXPECT_TRUE(VecNear(cross(z, x), y, kEpsilonF));
    }

    TEST(CrossProductTest, ReverseOrderNegation)
    {
        Vec3 a{1, 2, 3};
        Vec3 b{4, 5, 6};
        Vec3 ab = cross(a, b);
        Vec3 ba = cross(b, a);

        EXPECT_NEAR(ab.x, -ba.x, kEpsilonF);
        EXPECT_NEAR(ab.y, -ba.y, kEpsilonF);
        EXPECT_NEAR(ab.z, -ba.z, kEpsilonF);
    }

    TEST(CrossProductTest, ParallelVectorsZero)
    {
        Vec3 a{2, 2, 2};
        Vec3 b{4, 4, 4};
        Vec3 result = cross(a, b);

        EXPECT_NEAR(result.x, 0.0f, kEpsilonF);
        EXPECT_NEAR(result.y, 0.0f, kEpsilonF);
        EXPECT_NEAR(result.z, 0.0f, kEpsilonF);
    }

    TEST(TranslateTest, TranslatesIdentityMatrix)
    {
        Mat4 identity = Mat4::identity();
        Vec3 offset{2.0f, 3.0f, 4.0f};

        Mat4 translated = translate(identity, offset);

        Mat4 expected = Mat4::identity();
        expected[3][0] = offset.x;
        expected[3][1] = offset.y;
        expected[3][2] = offset.z;

        EXPECT_TRUE(MatNear(translated, expected, kEpsilonF));
    }

    TEST(ScaleTest, ScalesIdentityMatrixCorrectly)
    {
        Mat4 identity = Mat4::identity();
        Vec3 factors{2.0f, 3.0f, 4.0f};

        Mat4 scaled = scale(identity, factors);

        Mat4 expected = Mat4::identity();
        expected[0][0] = factors.x;
        expected[1][1] = factors.y;
        expected[2][2] = factors.z;

        EXPECT_TRUE(MatNear(scaled, expected, kEpsilonF));
    }
}