#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ChaiMath.h>

TEST(Mat4Tests, Identity)
{
    chai::Mat4 id = chai::Mat4::identity();
    chai::Mat4 mat =
    {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f
    };

    EXPECT_EQ(id, mat);
}

TEST(Mat4fTests, InitializerListConstructor) 
{
    chai::Mat4 mat = 
    {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };

	EXPECT_EQ(mat[0][0], 1);
    mat[0][0] = 100;
	EXPECT_EQ(mat[0][0], 100);

    chai::Vec4 row1 = mat[1];
    chai::Vec4 expected1 = { 5, 6, 7, 8 };
    EXPECT_EQ(row1, expected1);

    auto row2 = mat[2];
    row2 = { 90, 100, 110, 120 };

	EXPECT_EQ(mat[2][0], 90);
}