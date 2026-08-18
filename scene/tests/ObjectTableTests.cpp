#include <Scene/ObjectTable.h>
#include <Scene/Object.h>
#include <gtest/gtest.h>

namespace chai::scene
{
    TEST(ObjectTable, packAndUnpackId)
    {
        uint32_t id = 32;
        uint32_t generation = 1;

        uint64_t packed = pack({id, generation});

        const auto [idOut, genOut] = unpack(packed);

        EXPECT_EQ(idOut, id);
        EXPECT_EQ(genOut, generation);
    }

    class TestObj : public Object
    {
        ::chai::TypeInfo* typeInfo() const override
        {
            return ::chai::TypeRegistry::instance().getType<TestObj>().get();
        };
    };

    TEST(ObjectTable, AddRemoveIds)
    {
        TestObj obj;
        ObjectId id = obj.id();
        EXPECT_EQ(ObjectTable::instance().resolve(id), &obj);
        ObjectTable::instance().remove(id);
        EXPECT_EQ(ObjectTable::instance().resolve(id), nullptr);
        ObjectTable::instance().clear();
    }

    TEST(ObjectTable, ReuseSlots)
    {
        TestObj obj;
        ObjectId id = obj.id();
        const auto [idOut1, genOut1] = unpack(id);
        ObjectTable::instance().remove(id);

        TestObj obj2;
        ObjectId id2 = obj2.id();
        const auto [idOut2, genOut2] = unpack(id2);

        EXPECT_EQ(idOut1, idOut2);          //id should be reused
        EXPECT_EQ(genOut1 + 1, genOut2);    //generation is incremented
        ObjectTable::instance().clear();
    }
}