#include "test_helpers.h"

#include <ChaiMath.h>

namespace Mat4Tests
{
	using chai::Mat2;
	using chai::Mat4;
    using chai::Mat;
    using chai::EPSILON_F;

    using chai::Vec3;

    TEST(Mat4Tests, SizeAndAlign)
    {
        EXPECT_EQ(sizeof(Mat4), sizeof(float) * 16);
        EXPECT_EQ(alignof(Mat4), alignof(float));
    }

    TEST(Mat4Tests, Identity)
    {
        Mat4 id = Mat4::identity();
        Mat4 mat =
        {
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f
        };

        EXPECT_EQ(id, mat);
    }

    TEST(Mat4Tests, AssignmentAndEquality)
    {
        Mat4 mat{
            1, 2, 3, 4,
            5, 6, 7, 8,
            9, 10, 11, 12,
            13, 14, 15, 16
        };

        EXPECT_EQ(mat[0][0], 1);
        mat[0][0] = 100;
        EXPECT_EQ(mat[0][0], 100);

        chai::Vec4 row1 = mat[1];
        chai::Vec4 expected1{ 5, 6, 7, 8 };
        EXPECT_EQ(row1, expected1);
    }

    TEST(MatrixMultiplyTest, IdentityMatrix) 
    {
        Mat<float, 4, 4> A{
            1, 2, 3, 4,
            5, 6, 7, 8,
            9,10,11,12,
            13,14,15,16
        };

        // Transposed initializer (column-major) for simplicity
        for (int col = 0; col < 4; ++col)
            for (int row = 0; row < 4; ++row)
                A[col][row] = 1.0f + col * 4 + row;

        Mat<float, 4, 4> I = Mat<float, 4, 4>::identity();

        auto R = A * I;

        EXPECT_TRUE(MatNear(R, A, EPSILON_F));
    }

    TEST(MatrixMultiplyTest, MultiplyKnownValues) 
    {
        Mat2 A{ 1, 2, 3, 4 };
        Mat2 B{ 5, 6, 7, 8 };

        Mat<float, 2, 2> R = A * B;

        Mat<float, 2, 2> expected;
        expected[0][0] = 1 * 5 + 2 * 7;
        expected[0][1] = 1 * 6 + 2 * 8;
        expected[1][0] = 3 * 5 + 4 * 7;
        expected[1][1] = 3 * 6 + 4 * 8;

        EXPECT_TRUE(MatNear(R, expected, EPSILON_F));
    }
}