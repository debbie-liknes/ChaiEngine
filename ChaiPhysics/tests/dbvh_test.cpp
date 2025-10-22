#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ChaiPhysics/Core/dbvh.h>


// Test 1:
// - Construct DBVH
// - Test defaults of tree (bounds, overlapping pairs, etc.)

TEST(dbvh, construct)
{
    auto dbvh = chai::dbvh();

    // No overlapping pairs on construction
    EXPECT_THAT(dbvh.getOverlappingPairs(), ::testing::ElementsAre());
    EXPECT_THAT(dbvh.getRootNode(), ::testing::IsNull());
}

TEST(aabb, expand)
{
    auto aabb1 = chai::aabb{
        .center = chai::Vec3(0, 0, 0),
        .width = 1,
        .length = 1,
        .height = 1
    };
    auto aabb2 = chai::aabb{
        .center = chai::Vec3(0, 0, 0),
        .width = 1,
        .length = 1,
        .height = 1
    };

    EXPECT_THAT(aabb1.expand(aabb2), aabb1);

    auto aabb3 = chai::aabb{
        .center = chai::Vec3(0, 0, 0),
        .width = 2,
        .length = 2,
        .height = 2
    };

    EXPECT_THAT(aabb1.expand(aabb3), aabb3);

    auto aabb4 = chai::aabb{
        .center = chai::Vec3(1, 1, 1),
        .width = 1,
        .length = 1,
        .height = 1
    };
    auto aabb5 = chai::aabb{
        .center = chai::Vec3(0.5f, 0.5f, 0.5f),
        .width = 2,
        .length = 2,
        .height = 2
    };

    EXPECT_THAT(aabb1.expand(aabb4), aabb5);
}

// Test 2:
// - Construct DBVH
// - Insert single item
// - Test bounds == bounds of single item + padding
// - Test two nodes - root node and leaf
// - No overlapping pairs
TEST(dbvh, single_box)
{
    auto dbvh = chai::dbvh();

    // Insert a box at origin
    chai::BoxCollider box{
        .box {
            .center {
                0, 0, 0
            },
            .width { 1 },
            .length { 1 },
            .height { 1 }
        },
        .center { chai::Vec3(0, 0, 0) },
        .rot { chai::Quat(1, 0, 0, 0) }
    };
    dbvh.insert(&box);

    auto aabb = box.getWorldBounds();
    EXPECT_EQ(aabb.center, chai::Vec3(0, 0, 0));
    EXPECT_EQ(aabb.width, 1.0);
    EXPECT_EQ(aabb.length, 1.0);
    EXPECT_EQ(aabb.height, 1.0);

    auto rootNode = dbvh.getRootNode();
    ASSERT_THAT(rootNode, ::testing::NotNull());
    EXPECT_EQ(rootNode->object, &box);
    EXPECT_THAT(rootNode->left, ::testing::IsNull());
    EXPECT_THAT(rootNode->right, ::testing::IsNull());

    dbvh.remove(&box);
    EXPECT_THAT(dbvh.getRootNode(), ::testing::IsNull());
}

TEST(dbvh, insert_two_nodes_into_dbvh)
{
    auto dbvh = chai::dbvh();

    // Insert a box at origin
    chai::BoxCollider box1{
        .box {
            .center {
                0, 0, 0
            },
            .width { 1 },
            .length { 1 },
            .height { 1 }
        },
        .center { chai::Vec3(0, 0, 0) },
        .rot { chai::Quat(1, 0, 0, 0) }
    };
    chai::BoxCollider box2{
        .box {
            .center {
                1, 1, 1
            },
            .width { 1 },
            .length { 1 },
            .height { 1 }
        },
        .center { chai::Vec3(0, 0, 0) },
        .rot { chai::Quat(1, 0, 0, 0) }
    };
    dbvh.insert(&box1);
    dbvh.insert(&box2);

    auto rootNode = dbvh.getRootNode();
    ASSERT_THAT(rootNode, ::testing::NotNull());
    EXPECT_THAT(rootNode->object, ::testing::IsNull());
    ASSERT_THAT(rootNode->left, ::testing::NotNull());
    ASSERT_THAT(rootNode->right, ::testing::NotNull());

    EXPECT_EQ(rootNode->left->object, &box2);
    EXPECT_EQ(rootNode->right->object, &box1);

    ASSERT_THAT(rootNode->left->left, ::testing::IsNull());
    ASSERT_THAT(rootNode->left->right, ::testing::IsNull());
    ASSERT_THAT(rootNode->right->left, ::testing::IsNull());
    ASSERT_THAT(rootNode->right->right, ::testing::IsNull());

    dbvh.remove(&box1);
    dbvh.remove(&box2);
    EXPECT_THAT(dbvh.getRootNode(), ::testing::IsNull());
}
