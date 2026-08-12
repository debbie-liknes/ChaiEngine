/**
 * @file ModelLoaderPlugin.h
 * @brief Plugin setup and registration of services
 */
#include <Log.h>
#include "GLTFLoader.h"
#include <Plugin/PluginBase.h>
#include <Plugin/PluginMacros.h>
#include <Plugin/ServiceLocator.h>
#include <memory>

namespace chai::gfx
{
    class ModelLoaderPlugin : public IPlugin
    {
    public:
        const char* name() const override { return "Loader(gltf)"; }
        
        ServiceList requiredServices() const override { return {}; }
        ServiceList providedServices() const override { return { typeid(gfx::IModelLoader) }; }

        void onLoad(PluginContext& ctx) override
        {
            // register services, but make sure to UN-register them on unload
            ctx.services.provide<gfx::IModelLoader>(std::make_shared<GLTFLoader>());

            CHAI_LOG_INFO("Model Plugin loaded");
        }

        void onUnload(PluginContext& ctx) override
        {
            ctx.services.remove<gfx::IModelLoader>();
            CHAI_LOG_INFO("Model Plugin removed");
        }

    private:
    };

    CHAI_PLUGIN(ModelLoaderPlugin);
} // namespace chai