#include <TypeRegistry.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace chai
{
    struct Foo {
        int x = 0;
        void inc() { ++x; }
    };

    TEST(TypeRegistryTest, RegisterAndCreate)
    {
        TypeRegistry reg; // fresh, isolated
        reg.registerType<Foo>("Foo");

        auto info = reg.getType("Foo");
        ASSERT_NE(info, nullptr);
        EXPECT_EQ(info->size, sizeof(Foo));
        EXPECT_NE(reg.createInstance("Foo"), nullptr);
    }

    TEST(TypeRegistryTest, UnknownTypeReturnsNull)
    {
        TypeRegistry reg;
        EXPECT_EQ(reg.createInstance("Nope"), nullptr);
    }
}