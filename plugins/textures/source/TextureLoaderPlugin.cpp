/**
 * @file TextureLoaderPlugin.h
 * @brief Plugin setup and registration of services
 */
#include <Log.h>
#include <Plugin/PluginBase.h>
#include <Plugin/PluginMacros.h>
#include <Plugin/ServiceLocator.h>
#include <PNGLoader.h>
#include <memory>

namespace chai
{
    class TextureLoaderPlugin : public IPlugin
    {
    public:
        const char* name() const override { return "Renderer(Vulkan)"; }

        void onLoad(PluginContext& ctx) override
        {
            // register services, but make sure to UN-register them on unload
            ctx.services.provide<gfx::ITextureLoader>(std::make_shared<PNGLoader>());

            CHAI_LOG_INFO("Texture Plugin loaded");
        }

        void onUnload(PluginContext& ctx) override
        {
            ctx.services.remove<gfx::ITextureLoader>();
            CHAI_LOG_INFO("Texture Plugin removed");
        }

    private:
    };

    CHAI_PLUGIN(TextureLoaderPlugin);
}