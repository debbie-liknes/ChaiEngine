#pragma once
#include <string>

#include <ChaiMath.h>

class AudioEngine
{
public:
    static void Init();
    static void Update();
    static void Shutdown();

    void LoadSound(const std::string& strSoundName,
                   bool b3d = true, bool bLooping = false, bool bStream = false);
    void UnLoadSound(const std::string& strSoundName);
    int PlaySound(const std::string& strSoundName,
                  const chai::Vec3& vPos = chai::Vec3{0, 0, 0}, float fVolumedB = 0.0f);

    void Set3dListenerAndOrientation(const chai::Vec3& vPosition,
                                     const chai::Vec3& vLook, const chai::Vec3& vUp);
    void StopChannel(int nChannelId);
    void StopAllChannels();
    void SetChannel3dPosition(int nChannelId, const chai::Vec3& vPosition);
    void SetChannelVolume(int nChannelId, float fVolumedB);
    bool IsPlaying(int nChannelId) const;
};