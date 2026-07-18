#pragma once
#include <string>
#include <Audio/IAudioEngine.h>

namespace audio
{
    class AudioEngine : public IAudioEngine
    {
    public:
        void init() override;
        void update() override;
        void shutdown() override;

        void loadSound(const std::string& strSoundName,
                       bool b3d = true,
                       bool bLooping = false,
                       bool bStream = false) override;
        void unLoadSound(const std::string& strSoundName) override;
        int playSound(const std::string& strSoundName,
                      const chai::math::Vec3& vPos = chai::math::Vec3{0, 0, 0},
                      float fVolumedB = 0.0f) override;

        void set3dListenerAndOrientation(const chai::math::Vec3& vPosition,
                                         const chai::math::Vec3& vLook,
                                         const chai::math::Vec3& vUp) override;
        void stopChannel(int nChannelId) override;
        void stopAllChannels() override;
        void setChannel3dPosition(int nChannelId, const chai::math::Vec3& vPosition) override;
        void setChannelVolume(int nChannelId, float fVolumedB) override;
        bool isPlaying(int nChannelId) const override;
    };
}