/**
 * @file FmodAudioPlugin.h
 * @brief Plugin setup and registration of services
 */
#include <Log.h>
#include <AudioEngine.h>
#include <Plugin/PluginBase.h>
#include <Plugin/PluginMacros.h>
#include <Plugin/ServiceLocator.h>
#include <memory>

namespace chai
{
    class FmodAudioPlugin : public IPlugin
    {
    public:
        const char* name() const override { return "AudioEngine(fmod)"; }

        void onLoad(PluginContext& ctx) override
        {
            m_audioEngine = std::make_shared<audio::AudioEngine>();
            m_audioEngine->init();

            // register services, but make sure to UN-register them on unload
            ctx.services.provide<audio::IAudioEngine>(m_audioEngine);

            CHAI_LOG_INFO("Texture Plugin loaded");
        }

        void onUnload(PluginContext& ctx) override
        {
            m_audioEngine->shutdown();

            ctx.services.remove<audio::IAudioEngine>();
            CHAI_LOG_INFO("Texture Plugin removed");
        }

    private:
        std::shared_ptr<audio::AudioEngine> m_audioEngine;
    };

    CHAI_PLUGIN(FmodAudioPlugin);
} // namespace chai