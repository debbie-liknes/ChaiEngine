#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ChaiMath.h>

TEST(Vec2Tests, ConstructAndAccess)
{
    chai::Vec2 v(1.0f, 2.0f);

    EXPECT_FLOAT_EQ(v.x, 1.0f);
    EXPECT_FLOAT_EQ(v.y, 2.0f);
    EXPECT_FLOAT_EQ(v[0], 1.0f);
    EXPECT_FLOAT_EQ(v[1], 2.0f);
}

TEST(Vec2Tests, WriteComponent)
{
    chai::Vec2 v(1.0f, 2.0f);
	v[0] = 3.0f;
    float y = v[1];

    EXPECT_FLOAT_EQ(v.x, 3.0f);
    EXPECT_FLOAT_EQ(y, 2.0f);
    EXPECT_FLOAT_EQ(v[0], 3.0f);
}

TEST(Vec2Tests, InitializerList)
{
    chai::Vec2 v = { 4.0f, 5.0f };
    EXPECT_EQ(v, chai::Vec2(4.0f, 5.0f));
}

TEST(Vec2Tests, CopyAndMove)
{
    chai::Vec2 a(1.0f, 2.0f);
    chai::Vec2 b = a;
    EXPECT_EQ(a, b);

    chai::Vec2 c(std::move(a));
    EXPECT_EQ(c, b); // a is now moved-from, c should still be valid
}

TEST(Vec2Tests, AdditionOperator)
{
    chai::Vec2 a(1.0f, 2.0f);
    chai::Vec2 b(4.0f, 5.0f);
    chai::Vec2 result = a + b;
    EXPECT_EQ(result, chai::Vec2(5.0f, 7.0f));
}

TEST(Vec2Tests, SubtractionOperator) 
{
    chai::Vec2 a(5.0f, 7.0f);
    chai::Vec2 b(1.0f, 2.0f);
    chai::Vec2 result = a - b;
    EXPECT_EQ(result, chai::Vec2(4.0f, 5.0f));
}

TEST(Vec2Tests, UnaryMinusOperator) 
{
    chai::Vec2 v(1.0f, -2.0f);
    chai::Vec2 result = -v;
    EXPECT_EQ(result, chai::Vec2(-1.0f, 2.0f));
}

TEST(Vec2Tests, EqualityOperator) 
{
    chai::Vec2 a(1.0f, 2.0f);
    chai::Vec2 b(1.0f, 2.0f);
    chai::Vec2 c(3.0f, 2.0f);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}