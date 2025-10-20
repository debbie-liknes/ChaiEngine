#include "test_helpers.h"

#include <ChaiMath.h>

//Baisc tests for the specialized vecs and their operators

namespace vec2_tests
{
    using chai::Vec2;

    TEST(Vec2Tests, SizeAndAlign)
    {
        EXPECT_EQ(sizeof(Vec2), sizeof(float) * 2);
        EXPECT_EQ(alignof(Vec2), alignof(float));
    }

    TEST(Vec2Tests, ConstructAndAccess)
    {
        Vec2 v(1.0f, 2.0f);

        EXPECT_FLOAT_EQ(v.x, 1.0f);
        EXPECT_FLOAT_EQ(v.y, 2.0f);
        EXPECT_FLOAT_EQ(v[0], 1.0f);
        EXPECT_FLOAT_EQ(v[1], 2.0f);
    }

    TEST(Vec2Tests, VariadicConstructor)
    {
        Vec2 v(7.0f);
        EXPECT_FLOAT_EQ(v.x, 7.0f);
        EXPECT_FLOAT_EQ(v.y, 7.0f);
        EXPECT_FLOAT_EQ(v[0], 7.0f);
        EXPECT_FLOAT_EQ(v[1], 7.0f);
    }

    TEST(Vec2Tests, WriteComponent)
    {
        Vec2 v(1.0f, 2.0f);
        v[0] = 3.0f;
        float y = v[1];

        EXPECT_FLOAT_EQ(v.x, 3.0f);
        EXPECT_FLOAT_EQ(y, 2.0f);
        EXPECT_FLOAT_EQ(v[0], 3.0f);
    }

    TEST(Vec2Tests, InitializerList)
    {
        Vec2 v = { 4.0f, 5.0f };
        Vec2 v1{ 4.0f, 5.0f };
        EXPECT_EQ(v, Vec2(4.0f, 5.0f));
        EXPECT_EQ(v1, Vec2(4.0f, 5.0f));
    }

    TEST(Vec2Tests, CopyAndMove)
    {
        Vec2 a(1.0f, 2.0f);
        Vec2 b = a;
        EXPECT_EQ(a, b);

        Vec2 c(std::move(a));
        EXPECT_EQ(c, b); // a is now moved-from, c should still be valid
    }

    TEST(Vec2Tests, AdditionOperator)
    {
        Vec2 a(1.0f, 2.0f);
        Vec2 b(4.0f, 5.0f);
        Vec2 result = a + b;
        EXPECT_EQ(result, Vec2(5.0f, 7.0f));
    }

    TEST(Vec2Tests, SubtractionOperator)
    {
        Vec2 a(5.0f, 7.0f);
        Vec2 b(1.0f, 2.0f);
        Vec2 result = a - b;
        EXPECT_EQ(result, Vec2(4.0f, 5.0f));
    }

    TEST(Vec2Tests, UnaryMinusOperator)
    {
        Vec2 v(1.0f, -2.0f);
        Vec2 result = -v;
        EXPECT_EQ(result, Vec2(-1.0f, 2.0f));
    }

    TEST(Vec2Tests, EqualityOperator)
    {
        Vec2 a(1.0f, 2.0f);
        Vec2 b(1.0f, 2.0f);
        Vec2 c(3.0f, 2.0f);
        EXPECT_TRUE(a == b);
        EXPECT_FALSE(a == c);
    }
}

namespace vec3_tests
{
    using chai::Vec3;

    TEST(Vec3Tests, SizeAndAlign)
    {
        EXPECT_EQ(sizeof(chai::Vec3), sizeof(float) * 3);
        EXPECT_EQ(alignof(chai::Vec3), alignof(float));
    }

    TEST(Vec3Tests, ConstructAndAccess)
    {
        Vec3 v(1.0f, 2.0f, 3.0f);

        EXPECT_FLOAT_EQ(v.x, 1.0f);
        EXPECT_FLOAT_EQ(v.y, 2.0f);
        EXPECT_FLOAT_EQ(v.z, 3.0f);
        EXPECT_FLOAT_EQ(v[0], 1.0f);
        EXPECT_FLOAT_EQ(v[1], 2.0f);
        EXPECT_FLOAT_EQ(v[2], 3.0f);
    }

    TEST(Vec3Tests, WriteComponent)
    {
        Vec3 v(1.0f, 2.0f, 3.0f);
        v[0] = 3.0f;
        float y = v[1];

        EXPECT_FLOAT_EQ(v.x, 3.0f);
        EXPECT_FLOAT_EQ(y, 2.0f);
        EXPECT_FLOAT_EQ(v[0], 3.0f);
    }

    TEST(Vec3Tests, InitializerList)
    {
        Vec3 v = { 4.0f, 5.0f, 6.0f };
        EXPECT_EQ(v, Vec3(4.0f, 5.0f, 6.0f));
    }

    TEST(Vec3Tests, CopyAndMove)
    {
        Vec3 a(1.0f, 2.0f, 3.0f);
        Vec3 b = a;
        EXPECT_EQ(a, b);

        Vec3 c(std::move(a));
        EXPECT_EQ(c, b); // a is now moved-from, c should still be valid
    }

    TEST(Vec3Tests, AdditionOperator)
    {
        Vec3 a(1.0f, 2.0f, 3.0f);
        Vec3 b(4.0f, 5.0f, 6.0f);
        Vec3 result = a + b;
        EXPECT_EQ(result, Vec3(5.0f, 7.0f, 9.0f));
    }

    TEST(Vec3Tests, SubtractionOperator)
    {
        Vec3 a(5.0f, 7.0f, 9.0f);
        Vec3 b(1.0f, 2.0f, 3.0f);
        Vec3 result = a - b;
        EXPECT_EQ(result, Vec3(4.0f, 5.0f, 6.0f));
    }

    TEST(Vec3Tests, UnaryMinusOperator)
    {
        Vec3 v(1.0f, -2.0f, 3.0f);
        Vec3 result = -v;
        EXPECT_EQ(result, Vec3(-1.0f, 2.0f, -3.0f));
    }

    TEST(Vec3Tests, EqualityOperator)
    {
        Vec3 a(1.0f, 2.0f, 3.0f);
        Vec3 b(1.0f, 2.0f, 3.0f);
        Vec3 c(3.0f, 2.0f, 7.0f);
        EXPECT_TRUE(a == b);
        EXPECT_FALSE(a == c);
    }
}

namespace vec4_tests
{
    using chai::Vec4;

    TEST(Vec4Tests, SizeAndAlign)
    {
        EXPECT_EQ(sizeof(chai::Vec4), sizeof(float) * 4);
        EXPECT_EQ(alignof(chai::Vec4), alignof(float));
    }

    TEST(Vec4Tests, ConstructAndAccess)
    {
        Vec4 v(1.0f, 2.0f, 3.0f, 4.0f);

        EXPECT_FLOAT_EQ(v.x, 1.0f);
        EXPECT_FLOAT_EQ(v.y, 2.0f);
        EXPECT_FLOAT_EQ(v.z, 3.0f);
        EXPECT_FLOAT_EQ(v.w, 4.0f);
        EXPECT_FLOAT_EQ(v[0], 1.0f);
        EXPECT_FLOAT_EQ(v[1], 2.0f);
        EXPECT_FLOAT_EQ(v[2], 3.0f);
        EXPECT_FLOAT_EQ(v[3], 4.0f);
    }

    TEST(Vec4Tests, WriteComponent)
    {
        Vec4 v(1.0f, 2.0f, 3.0f, 4.0);
        v[0] = 3.0f;
        float y = v[1];

        EXPECT_FLOAT_EQ(v.x, 3.0f);
        EXPECT_FLOAT_EQ(y, 2.0f);
        EXPECT_FLOAT_EQ(v[0], 3.0f);
    }

    TEST(Vec4Tests, InitializerList)
    {
        Vec4 v = { 4.0f, 5.0f, 6.0f, 7.0f };
        EXPECT_EQ(v, Vec4(4.0f, 5.0f, 6.0f, 7.0));
    }

    TEST(Vec4Tests, CopyAndMove)
    {
        Vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
        Vec4 b = a;
        EXPECT_EQ(a, b);

        Vec4 c(std::move(a));
        EXPECT_EQ(c, b); // a is now moved-from, c should still be valid
    }

    TEST(Vec4Tests, AdditionOperator)
    {
        Vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
        Vec4 b(4.0f, 5.0f, 6.0f, 4.0f);
        Vec4 result = a + b;
        EXPECT_EQ(result, Vec4(5.0f, 7.0f, 9.0f, 8.0f));
    }

    TEST(Vec4Tests, SubtractionOperator)
    {
        Vec4 a(5.0f, 7.0f, 9.0f, 10.f);
        Vec4 b(1.0f, 2.0f, 3.0f, 4.0f);
        Vec4 result = a - b;
        EXPECT_EQ(result, Vec4(4.0f, 5.0f, 6.0f, 6.0f));
    }

    TEST(Vec4Tests, UnaryMinusOperator)
    {
        Vec4 v(1.0f, -2.0f, 3.0f, -4.0f);
        Vec4 result = -v;
        EXPECT_EQ(result, Vec4(-1.0f, 2.0f, -3.0f, 4.0f));
    }

    TEST(Vec4Tests, EqualityOperator)
    {
        Vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
        Vec4 b(1.0f, 2.0f, 3.0f, 4.0f);
        Vec4 c(3.0f, 2.0f, 7.0f, 4.0f);
        EXPECT_TRUE(a == b);
        EXPECT_FALSE(a == c);
    }
}