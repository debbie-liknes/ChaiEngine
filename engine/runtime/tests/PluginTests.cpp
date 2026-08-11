#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Plugin/ServiceLocator.h>
#include <TypeRegistry.h>
#include <Core/Engine.h>
#include <Plugin/PluginLoader.h>


namespace chai
{
    using testing::_;

    class MockPlugin : public IPlugin
    {
    public:
        MOCK_METHOD(const char*, name, (), (const, override));
        MOCK_METHOD(void, onLoad, (PluginContext&), (override));
        MOCK_METHOD(void, onUnload, (PluginContext&), (override));
    };

    class IPhysics
    {
    public:
        virtual ~IPhysics() = default;

        virtual void step(float dt) = 0;
        virtual void setGravity(float g) = 0;
        virtual int bodyCount() const = 0;
    };

    class SimplePhysics : public IPhysics
    {
    public:
        void step(float dt) override { time_ += dt; }
        void setGravity(float g) override { gravity_ = g; }
        int bodyCount() const override { return 0; }

    private:
        float gravity_ = -9.81f;
        float time_ = 0.0f;
    };

    class PhysicsPlugin : public IPlugin
    {
    public:
        const char* name() const override { return "Physics"; }

        void onLoad(PluginContext& ctx) override
        {
            ctx.services.provide<IPhysics>(std::make_shared<SimplePhysics>());
        }

        void onUnload(PluginContext& ctx) override
        {
            ctx.services.remove<IPhysics>();
        }
    };

    TEST(PhysicsPluginTest, ProvidesAndRemovesService)
    {
        ServiceLocator services;
        TypeRegistry types;
        PluginContext ctx{services, types};

        PhysicsPlugin plugin;

        plugin.onLoad(ctx);
        EXPECT_NE(services.tryResolve<IPhysics>(), nullptr);

        plugin.onUnload(ctx);
        EXPECT_EQ(services.tryResolve<IPhysics>(), nullptr);
    }
}