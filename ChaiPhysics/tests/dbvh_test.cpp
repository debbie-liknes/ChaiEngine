#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../core/dbvh.h"


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
    chai::dbvh::BoxCollider box{
        .box {
            .center {
                0, 0, 0
            },
            .width { 1 },
            .length { 1 },
            .height { 1 }
        },
        .center { glm::vec3(0, 0, 0) },
        .rot { glm::quat(1, 0, 0, 0) }
    };
    dbvh.insert(&box);

    auto aabb = box.getWorldBounds();
    EXPECT_EQ(aabb.center, glm::vec3(0, 0, 0));
    EXPECT_EQ(aabb.width, 1.0);
    EXPECT_EQ(aabb.length, 1.0);
    EXPECT_EQ(aabb.height, 1.0);

    // No overlapping pairs on construction
    EXPECT_THAT(dbvh.getOverlappingPairs(), ::testing::ElementsAre());
    ASSERT_THAT(dbvh.getRootNode(), ::testing::NotNull());
}
