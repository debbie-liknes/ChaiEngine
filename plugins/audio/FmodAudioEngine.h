#pragma once
#include <string>
#include <Audio/IAudioEngine.h>

namespace chai::audio::fmod
{
    class FmodAudioEngine : public IAudioEngine
    {
    public:
        FmodAudioEngine();
        ~FmodAudioEngine();

        void update() override;

        void loadSound(const std::string& strSoundName,
                       bool b3d = true,
                       bool bLooping = false,
                       bool bStream = false) override;
        void unLoadSound(const std::string& strSoundName) override;
        int playSound(const std::string& strSoundName,
                      const chai::math::Vec3& vPos = chai::math::Vec3{0, 0, 0},
                      float fVolumedB = 0.0f) override;

        void set3dListenersAndOrientations(const AudioSceneData& audioData) override;
        void stopChannel(int nChannelId) override;
        void stopAllChannels() override;
        void setChannel3dPosition(int nChannelId, const chai::math::Vec3& vPosition) override;
        void setChannelVolume(int nChannelId, float fVolumedB) override;
        bool isPlaying(int nChannelId) const override;

    private:
        class Implementation;
        std::unique_ptr<Implementation> sgpImplementation_;
    };
}