#include "AudioEngine.h"

#include <vector>
#include <map>
#include <cmath>

#ifdef FMOD_FOUND
    #include <fmod.hpp>
#endif

inline float dBToVolume(float dB)
{
    return std::pow(10.0f, dB / 20.0f);
}

inline float volumeToDb(float volume)
{
    return 20.0f * std::log10(std::max(volume, 1e-6f)); // avoid log(0)
}

#ifdef FMOD_FOUND
inline FMOD_VECTOR vec3ToFmod(const glm::vec3& vec)
{
    return { vec.x, vec.y, vec.z };
}
#endif

struct Implementation
{
    Implementation()
    {
#ifdef FMOD_FOUND
        FMOD::System_Create(&mpSystem);

        auto flags = FMOD_INIT_NORMAL;
#ifdef _DEBUG
        flags |= FMOD_INIT_PROFILE_ENABLE;
#endif
        mpSystem->init(512, flags, nullptr);
#endif
    }
    ~Implementation() {}

    void Update();

#ifdef FMOD_FOUND
    FMOD::System* mpSystem = nullptr;

    int mnNextChannelId = 0;

    typedef std::map<std::string, FMOD::Sound*> SoundMap;
    typedef std::map<int, FMOD::Channel*> ChannelMap;
    SoundMap mSounds;
    ChannelMap mChannels;
#endif
};

void Implementation::Update()
{
#ifdef FMOD_FOUND
    std::vector<ChannelMap::iterator> pStoppedChannels;
    for (auto it = mChannels.begin(),
        itEnd = mChannels.end(); it != itEnd; ++it)
    {
        bool bIsPlaying = false;
        it->second->isPlaying(&bIsPlaying);
        if (!bIsPlaying)
        {
            pStoppedChannels.push_back(it);
        }
    }

    for (auto& it : pStoppedChannels)
    {
        mChannels.erase(it);
    }
    mpSystem->update();
#endif
}


Implementation* sgpImplementation = nullptr;

void AudioEngine::Init()
{
    sgpImplementation = new Implementation;
}

void AudioEngine::Update()
{
    sgpImplementation->Update();
}

void AudioEngine::Shutdown()
{
    delete sgpImplementation;
}

void AudioEngine::LoadSound(const std::string& strSoundName, bool b3d,
    bool bLooping, bool bStream)
{
#ifdef FMOD_FOUND
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt != sgpImplementation->mSounds.end())
        return;

    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= b3d ? FMOD_3D : FMOD_2D;
    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    FMOD::Sound* pSound = nullptr;
    sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode,
        nullptr, &pSound);
    if (pSound)
    {
        sgpImplementation->mSounds[strSoundName] = pSound;
    }
#endif
}

void AudioEngine::UnLoadSound(const std::string& strSoundName)
{
#ifdef FMOD_FOUND
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->mSounds.end())
        return;
    tFoundIt->second->release();
    sgpImplementation->mSounds.erase(tFoundIt);
#endif
}

int AudioEngine::PlaySound(const std::string& strSoundName,
    const glm::vec3& vPosition, float fVolumedB)
{
#ifdef FMOD_FOUND
    int nChannelId = sgpImplementation->mnNextChannelId++;
    auto tFoundId = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundId == sgpImplementation->mSounds.end())
    {
        LoadSound(strSoundName);
        tFoundId = sgpImplementation->mSounds.find(strSoundName);
        if (tFoundId == sgpImplementation->mSounds.end())
        {
            return nChannelId;
        }
    }

    FMOD::Channel* pChannel = nullptr;
    sgpImplementation->mpSystem->playSound(tFoundId->second, nullptr, true, &pChannel);
    if (pChannel)
    {
        FMOD_VECTOR position = vec3ToFmod(vPosition);
        pChannel->set3DAttributes(&position, nullptr);
        pChannel->setVolume(dBToVolume(fVolumedB));
        pChannel->setPaused(false);
        sgpImplementation->mChannels[nChannelId] = pChannel;
    }
    return nChannelId;
#else
    return 0;
#endif
}

void AudioEngine::Set3dListenerAndOrientation(const glm::vec3& vPosition,
    const glm::vec3& vLook, const glm::vec3& vUp)
{
#ifdef FMOD_FOUND
    auto pos = vec3ToFmod(vPosition);
    pos.x = -pos.x;
    pos.z = -pos.z;
    auto look = vec3ToFmod(vLook);
    auto up = vec3ToFmod(vUp);

    sgpImplementation->mpSystem->set3DListenerAttributes(
        0,
        &pos,
        0,
        &look,
        &up);
#endif
}

#ifdef FMOD_FOUND
// Checks that idx is 0 <= n < size and returns 'val' if failed.
// Asserts if a failure occurs in Debug mode.
#define CHECK_BOUNDS(idx, iterable, val) \
    do \
    { \
        if (nChannelId < 0 || \
            nChannelId >= sgpImplementation->mChannels.size()) \
        { \
            assert(0); \
            return val; \
        } \
    } while (0)
#endif

void AudioEngine::StopChannel(int nChannelId)
{
#ifdef FMOD_FOUND
    CHECK_BOUNDS(nChannelId, sgpImplementation->mChannels,);
    sgpImplementation->mChannels[nChannelId]->stop();
#endif
}

void AudioEngine::StopAllChannels()
{
#ifdef FMOD_FOUND
    for (auto&& channel : sgpImplementation->mChannels)
    {
        channel.second->stop();
    }
#endif
}

void AudioEngine::SetChannel3dPosition(int nChannelId,
    const glm::vec3& vPosition)
{
#ifdef FMOD_FOUND
    CHECK_BOUNDS(nChannelId, sgpImplementation->mChannels,);

    const FMOD_VECTOR pos = vec3ToFmod(vPosition);
    const FMOD_VECTOR vel = FMOD_VECTOR(0, 0, 0);
    sgpImplementation->mChannels[nChannelId]->set3DAttributes(
        &pos,
        &vel
    );
#endif
}

void AudioEngine::SetChannelVolume(int nChannelId, float fVolumedB)
{
#ifdef FMOD_FOUND
    CHECK_BOUNDS(nChannelId, sgpImplementation->mChannels,);
    sgpImplementation->mChannels[nChannelId]->setVolume(dBToVolume(fVolumedB));
#endif
}

bool AudioEngine::IsPlaying(int nChannelId) const
{
#ifdef FMOD_FOUND
    CHECK_BOUNDS(nChannelId, sgpImplementation->mChannels, false);

    bool isPlaying = false;

    sgpImplementation->mChannels[nChannelId]->isPlaying(&isPlaying);
    return isPlaying;
#else
    return false;
#endif
}
