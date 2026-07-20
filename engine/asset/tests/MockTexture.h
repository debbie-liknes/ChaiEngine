#pragma once

// A fake asset domain for the cache tests. No Vulkan: the "resource" is just an
// int id and a live flag.

#include <AssetCache.h>

#include <gmock/gmock.h>

struct MockTex {
};

struct MockAsset {
    bool loaded = false;
    int width = 0;
};

struct MockResource {
    int id = 0;
    bool live = false;
};

template <>
struct chai::AssetTraits<MockTex> {
    using Asset = MockAsset;
    using Resource = MockResource;
};

class MockFactory : public chai::ResourceFactory<MockTex>
{
public:
    //MOCK_METHOD(bool, loadAsset, (chai::AssetId id, MockAsset& out), (override));
    MOCK_METHOD(chai::LoadState,
                createResource,
                (const MockAsset& asset, MockResource& out),
                (override));
    MOCK_METHOD(chai::LoadState, pollState, (const MockResource& res), (override));
    MOCK_METHOD(void, destroyResource, (MockResource & res), (noexcept, override));
    MOCK_METHOD(bool, discardAssetAfterUpload, (), (const, noexcept, override));
};