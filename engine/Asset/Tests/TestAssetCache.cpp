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

    TEST_F(AssetCacheTest, IngestCreatesResource)
    {
        MockAsset asset;
        asset.loaded = true;

        EXPECT_CALL(factory, createResource(_, _)).Times(1);

        auto h = cache.ingest(idA(), asset);

        EXPECT_TRUE(h.valid());
        EXPECT_TRUE(!cache.isReady(h));
        cache.tick();
        EXPECT_TRUE(cache.isReady(h));
        EXPECT_NE(cache.resource(h), nullptr);
    }

    TEST_F(AssetCacheTest, AcquireReturnsExistingHandle)
    {
        MockAsset asset;
        asset.loaded = true;

        auto h1 = cache.ingest(idA(), asset);
        auto h2 = cache.acquire(idA());

        EXPECT_EQ(h1, h2);
    }

    TEST_F(AssetCacheTest, ResourceDestroyedOnLastRelease)
    {
        MockAsset asset;
        asset.loaded = true;

        auto h = cache.ingest(idA(), asset);

        EXPECT_CALL(factory, destroyResource(_)).Times(1);

        cache.release(h);
        deleteQueue.flushAll();
    }

    TEST_F(AssetCacheTest, ResourceNotDestroyedUntilLastReference)
    {
        MockAsset asset;

        auto h1 = cache.ingest(idA(), asset);
        auto h2 = cache.addRef(h1);

        EXPECT_CALL(factory, destroyResource(_)).Times(0);

        cache.release(h1);

        deleteQueue.flushAll();

        EXPECT_CALL(factory, destroyResource(_)).Times(1);

        cache.release(h2);
        deleteQueue.flushAll();
    }

    TEST_F(AssetCacheTest, TickPromotesUploadingResource)
    {
        MockAsset asset;

        EXPECT_CALL(factory, createResource(_, _)).WillOnce(Return(LoadState::Uploading));

        auto handle = cache.acquire(idA());
        EXPECT_EQ(cache.state(handle), LoadState::Loading);

        auto h = cache.ingest(idA(), asset);
        EXPECT_EQ(cache.state(h), LoadState::Queued);

        cache.tick();

        EXPECT_EQ(cache.state(h), LoadState::Ready);
    }

    TEST_F(AssetCacheTest, FailedCreationProducesFailedState)
    {
        MockAsset asset;

        EXPECT_CALL(factory, createResource(_, _)).WillOnce(Return(LoadState::Failed));

        auto h = cache.ingest(idA(), asset);
        cache.tick();
        EXPECT_EQ(cache.state(h), LoadState::Failed);
    }

    TEST_F(AssetCacheTest, AssetRetainedWhenFactoryRequestsIt)
    {
        MockAsset asset;
        asset.width = 512;

        EXPECT_CALL(factory, discardAssetAfterUpload()).WillRepeatedly(Return(false));

        auto h = cache.ingest(idA(), asset);

        EXPECT_NE(cache.asset(h), nullptr);
    }

    TEST_F(AssetCacheTest, InvalidHandleReturnsNullResource)
    {
        Handle<MockTex> invalid{};

        EXPECT_EQ(cache.resource(invalid), nullptr);
    }
} // namespace