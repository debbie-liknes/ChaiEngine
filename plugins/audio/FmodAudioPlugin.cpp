/**
 * @file FmodAudioPlugin.h
 * @brief Plugin setup and registration of services
 */
#include <Log.h>
#include <FmodAudioEngine.h>
#include <Plugin/PluginBase.h>
#include <Plugin/PluginMacros.h>
#include <Plugin/ServiceLocator.h>
#include <memory>

namespace chai::audio::fmod
{
    class FmodAudioPlugin : public IPlugin
    {
    public:
        const char* name() const override { return "AudioEngine(fmod)"; }

        ServiceList requiredServices() const override { return {}; }
        ServiceList providedServices() const override { return {typeid(audio::IAudioEngine)}; }

        [[nodiscard]] bool onLoad(PluginContext& ctx) override
        {
            m_audioEngine = std::make_shared<FmodAudioEngine>();

            // register services, but make sure to UN-register them on unload
            ctx.services.provide<IAudioEngine>(m_audioEngine);

            CHAI_LOG_INFO("FMOD Audio Plugin loaded");
            
            return true;
        }

        void onUnload(PluginContext& ctx) override
        {
            ctx.services.remove<IAudioEngine>();
            CHAI_LOG_INFO("FMOD Audio Plugin removed");
        }

    private:
        std::shared_ptr<IAudioEngine> m_audioEngine;
    };

    CHAI_PLUGIN(FmodAudioPlugin);
} // namespace chai