#pragma once
#include <string>

#include <ChaiMath.h>

namespace audio
{
    class IAudioEngine
    {
    public:
        virtual ~IAudioEngine() = default;

        virtual void init() = 0;
        virtual void update() = 0;
        virtual void shutdown() = 0;

        virtual void loadSound(const std::string& strSoundName,
                       bool b3d = true,
                       bool bLooping = false,
                       bool bStream = false) = 0;
        virtual void unLoadSound(const std::string& strSoundName) = 0;
        virtual int playSound(const std::string& strSoundName,
                      const chai::math::Vec3& vPos = chai::math::Vec3{0, 0, 0},
                      float fVolumedB = 0.0f) = 0;

        virtual void set3dListenerAndOrientation(const chai::math::Vec3& vPosition,
                                         const chai::math::Vec3& vLook,
                                         const chai::math::Vec3& vUp) = 0;
        virtual void stopChannel(int nChannelId) = 0;
        virtual void stopAllChannels() = 0;
        virtual void setChannel3dPosition(int nChannelId, const chai::math::Vec3& vPosition) = 0;
        virtual void setChannelVolume(int nChannelId, float fVolumedB) = 0;
        virtual bool isPlaying(int nChannelId) const = 0;
    };
}