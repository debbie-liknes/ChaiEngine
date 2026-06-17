#include "MockTexture.h"

#include <cstdint>
#include <gtest/gtest.h>

using chai::AssetId;
using chai::LoadState;
using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;

namespace chai
{
    //Test cache
    class AssetCacheTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            ON_CALL(factory, createResource(_, _))
                .WillByDefault(Invoke([this](const MockAsset&, MockResource& r) {
                    r.id = ++lastId;
                    r.live = true;
                    return LoadState::Ready;
                }));
            ON_CALL(factory, pollState(_)).WillByDefault(Return(LoadState::Ready));
            ON_CALL(factory, discardAssetAfterUpload()).WillByDefault(Return(true));
        }

        AssetId idA() const { return chai::makeAssetId("a"); }
        AssetId idB() const { return chai::makeAssetId("b"); }

        chai::DeferredDeleteQueue deleteQueue;
        NiceMock<MockFactory> factory;
        chai::AssetCache<MockTex> cache{factory, deleteQueue};
        int lastId = 0;
    };

    TEST_F(AssetCacheTest, AcquireSameIdAsset)
    {
        EXPECT_CALL(factory, createResource(_, _)).Times(1);

        auto a = cache.acquire(idA());
        auto b = cache.acquire(idA());

        //Expect 2 of the same handle, but only 1 live asset
        EXPECT_EQ(a, b);
        EXPECT_EQ(cache.liveAssets(), 1u);

        // Release 1 asset, expect it to still be alive until the last ref is released
        cache.release(a);
        EXPECT_EQ(cache.liveAssets(), 1u);
        EXPECT_TRUE(cache.isReady(b));
        cache.release(b);

        // Now the last ref is gone, expect the asset to be gone too
        EXPECT_EQ(cache.liveAssets(), 0u);
    }

    TEST_F(AssetCacheTest, DifferentIdsGetDifferentSlots)
    {
        auto a = cache.acquire(idA());
        auto b = cache.acquire(idB());
        EXPECT_NE(a, b);
        EXPECT_EQ(cache.liveAssets(), 2u);
    }

    TEST_F(AssetCacheTest, SynchronousFactoryIsReadyImmediately)
    {
        auto h = cache.acquire(idA());
        EXPECT_EQ(cache.state(h), LoadState::Ready);
        EXPECT_TRUE(cache.isReady(h));
        ASSERT_NE(cache.resource(h), nullptr);
        EXPECT_TRUE(cache.resource(h)->live);
    }

    TEST_F(AssetCacheTest, AsyncFactory)
    {
        EXPECT_CALL(factory, createResource(_, _))
            .WillOnce(Invoke([this](const MockAsset&, MockResource& r) {
                r.id = ++lastId;
                r.live = true;
                return LoadState::Uploading;
            }));
        EXPECT_CALL(factory, pollState(_))
            .WillOnce(Return(LoadState::Uploading))
            .WillRepeatedly(Return(LoadState::Ready));

        auto h = cache.acquire(idA());
        EXPECT_EQ(cache.state(h), LoadState::Uploading);
        EXPECT_EQ(cache.resource(h), nullptr);
        EXPECT_EQ(cache.pendingUploads(), 1u);

        cache.tick();
        EXPECT_EQ(cache.state(h), LoadState::Uploading);
        EXPECT_EQ(cache.pendingUploads(), 1u);

        cache.tick();
        EXPECT_TRUE(cache.isReady(h));
        ASSERT_NE(cache.resource(h), nullptr);
        EXPECT_EQ(cache.pendingUploads(), 0u);
    }

    TEST_F(AssetCacheTest, FailedLoad)
    {
        EXPECT_CALL(factory, createResource(_, _)).Times(0);

        auto h = cache.acquire(idA());
        EXPECT_EQ(cache.state(h), LoadState::Failed);
        EXPECT_FALSE(cache.isReady(h));
        EXPECT_EQ(cache.resource(h), nullptr);
    }

    TEST_F(AssetCacheTest, DestroyResource)
    {
        EXPECT_CALL(factory, destroyResource(_)).Times(1);

        deleteQueue.beginFrame(0);
        auto h = cache.acquire(idA());
        cache.release(h);

        EXPECT_EQ(cache.resource(h), nullptr);
        EXPECT_EQ(cache.liveAssets(), 0u);
        EXPECT_EQ(deleteQueue.pendingCount(), 1u);

        deleteQueue.collect(0);
        EXPECT_EQ(deleteQueue.pendingCount(), 0u);
    }

    TEST_F(AssetCacheTest, ReleasingMidUpload)
    {
        EXPECT_CALL(factory, createResource(_, _))
            .WillOnce(Invoke([this](const MockAsset&, MockResource& r) {
                r.id = ++lastId;
                r.live = true;
                return LoadState::Uploading;
            }));
        EXPECT_CALL(factory, destroyResource(_)).Times(1);

        deleteQueue.beginFrame(0);
        auto h = cache.acquire(idA());
        EXPECT_EQ(cache.pendingUploads(), 1u);

        cache.release(h); // released before we finish
        EXPECT_EQ(cache.liveAssets(), 0u);
        cache.tick();
        EXPECT_EQ(cache.pendingUploads(), 0u);

        deleteQueue.collect(0);
    }

    TEST_F(AssetCacheTest, ReloadSwapsResourceUnderStableHandle)
    {
        EXPECT_CALL(factory, createResource(_, _)).Times(2);
        EXPECT_CALL(factory, destroyResource(_)).Times(1);

        deleteQueue.beginFrame(0);
        auto h = cache.acquire(idA());
        ASSERT_NE(cache.resource(h), nullptr);
        int firstId = cache.resource(h)->id;

        cache.reload(idA());
        cache.tick();

        EXPECT_TRUE(cache.isReady(h));
        ASSERT_NE(cache.resource(h), nullptr);
        EXPECT_NE(cache.resource(h)->id, firstId); // we actually got a new resource
        EXPECT_EQ(deleteQueue.pendingCount(), 1u);

        deleteQueue.collect(0);
    }

    TEST_F(AssetCacheTest, HandleIsUnusableAfterFullRelease)
    {
        //create then immediately realease. The handle is now stale
        auto h = cache.acquire(idA());
        cache.release(h);

        EXPECT_EQ(cache.state(h), LoadState::Empty);
        EXPECT_FALSE(cache.isReady(h));
        EXPECT_EQ(cache.resource(h), nullptr);
        EXPECT_EQ(cache.asset(h), nullptr);

        auto h2 = cache.acquire(idA()); // fresh slot for same id
        EXPECT_TRUE(cache.isReady(h2));
        EXPECT_FALSE(cache.isReady(h)); // stale handle never resurrects
    }

    TEST_F(AssetCacheTest, EveryCreatedResourceIsEventuallyDestroyed)
    {
        EXPECT_CALL(factory, createResource(_, _)).Times(3);
        EXPECT_CALL(factory, destroyResource(_)).Times(3);   // a, a, b

        deleteQueue.beginFrame(0);
        auto a = cache.acquire(idA());
        auto b = cache.acquire(idB());
        cache.reload(idA()); // retires a's first resource, creates a second
        cache.release(a);
        cache.release(b);

        deleteQueue.collect(0);
        EXPECT_EQ(deleteQueue.pendingCount(), 0u);
    }

} // namespace