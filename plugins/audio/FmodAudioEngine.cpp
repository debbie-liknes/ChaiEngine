#include "FmodAudioEngine.h"

#include <vector>
#include <map>
#include <assert.h>

#include <fmod.hpp>

static inline float dBToVolume(float dB)
{
    return std::pow(10.0f, dB / 20.0f);
}

static inline float volumeToDb(float volume)
{
    return 20.0f * std::log10(std::max(volume, 1e-6f)); // avoid log(0)
}

static inline FMOD_VECTOR vec3ToFmod(const chai::math::Vec3& vec)
{
    return {vec.x, vec.y, vec.z};
}

namespace chai::audio::fmod
{

    struct FmodAudioEngine::Implementation {
        Implementation()
        {
            FMOD::System_Create(&mpSystem);

            auto flags = FMOD_INIT_NORMAL;
    #ifdef _DEBUG
            flags |= FMOD_INIT_PROFILE_ENABLE;
    #endif
            mpSystem->init(512, flags, nullptr);
        }

        ~Implementation() = default;

        void Update();

        FMOD::System* mpSystem = nullptr;

        int mnNextChannelId = 0;

        using SoundMap = std::map<std::string, FMOD::Sound*, std::less<>>;
        using ChannelMap = std::map<int, FMOD::Channel*>;
        SoundMap mSounds;
        ChannelMap mChannels;
    };

    void FmodAudioEngine::Implementation::Update()
    {
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

        for (const auto& it : pStoppedChannels)
        {
            mChannels.erase(it);
        }
        mpSystem->update();
    }

    FmodAudioEngine::FmodAudioEngine() : IAudioEngine(),
        sgpImplementation_(std::make_unique<Implementation>())
    {
    }

    FmodAudioEngine::~FmodAudioEngine() = default;

    void FmodAudioEngine::update()
    {
        sgpImplementation_->Update();
    }

    void FmodAudioEngine::loadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
    {
        if (auto tFoundIt = sgpImplementation_->mSounds.find(strSoundName);
            tFoundIt != sgpImplementation_->mSounds.end())
            return;

        FMOD_MODE eMode = FMOD_DEFAULT;
        eMode |= b3d ? FMOD_3D : FMOD_2D;
        eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
        eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

        FMOD::Sound* pSound = nullptr;
        sgpImplementation_->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound);
        if (pSound) {
            sgpImplementation_->mSounds[strSoundName] = pSound;
        }
    }

    void FmodAudioEngine::unLoadSound(const std::string& strSoundName)
    {
        auto tFoundIt = sgpImplementation_->mSounds.find(strSoundName);
        if (tFoundIt == sgpImplementation_->mSounds.end())
            return;
        tFoundIt->second->release();
        sgpImplementation_->mSounds.erase(tFoundIt);
    }

    int FmodAudioEngine::playSound(const std::string& strSoundName,
                               const chai::math::Vec3& vPosition,
                               float fVolumedB)
    {
        int nChannelId = sgpImplementation_->mnNextChannelId++;
        auto tFoundId = sgpImplementation_->mSounds.find(strSoundName);
        if (tFoundId == sgpImplementation_->mSounds.end()) {
            loadSound(strSoundName, true, true);
            tFoundId = sgpImplementation_->mSounds.find(strSoundName);
            if (tFoundId == sgpImplementation_->mSounds.end()) {
                return nChannelId;
            }
        }

        FMOD::Channel* pChannel = nullptr;
        sgpImplementation_->mpSystem->playSound(tFoundId->second, nullptr, true, &pChannel);
        if (pChannel) {
            FMOD_VECTOR position = vec3ToFmod(vPosition);
            pChannel->set3DAttributes(&position, nullptr);
            pChannel->setVolume(dBToVolume(fVolumedB));
            pChannel->setPaused(false);
            sgpImplementation_->mChannels[nChannelId] = pChannel;
        }
        return nChannelId;
    }

    void FmodAudioEngine::set3dListenersAndOrientations(const AudioSceneData& audioData)
    {
        for (const auto& listener : audioData.listeners) {
            auto pos = vec3ToFmod(listener.pos);
            pos.x = -pos.x;
            pos.z = -pos.z;
            auto look = vec3ToFmod(listener.lookAt);
            auto up = vec3ToFmod(listener.up);

            sgpImplementation_->mpSystem->set3DListenerAttributes(listener.id, &pos, nullptr, &look, &up);
        }
    }

// Checks that idx is 0 <= n < size and returns 'val' if failed.
// Asserts if a failure occurs in Debug mode.
#define CHECK_BOUNDS(idx, iterable, val)                                                           \
    do {                                                                                           \
        if (nChannelId < 0 || nChannelId >= sgpImplementation_->mChannels.size()) {                 \
            assert(0);                                                                             \
            return val;                                                                            \
        }                                                                                          \
    } while (0)

    void FmodAudioEngine::stopChannel(int nChannelId)
    {
        CHECK_BOUNDS(nChannelId, sgpImplementation_->mChannels, );
        sgpImplementation_->mChannels[nChannelId]->stop();
    }

    void FmodAudioEngine::stopAllChannels()
    {
        for (const auto& [first, second] : sgpImplementation_->mChannels) {
            second->stop();
        }
    }

    void FmodAudioEngine::setChannel3dPosition(int nChannelId, const chai::math::Vec3& vPosition)
    {
        CHECK_BOUNDS(nChannelId, sgpImplementation_->mChannels, );

        const FMOD_VECTOR pos = vec3ToFmod(vPosition);
        const auto vel = FMOD_VECTOR{0, 0, 0};
        sgpImplementation_->mChannels[nChannelId]->set3DAttributes(&pos, &vel);
    }

    void FmodAudioEngine::setChannelVolume(int nChannelId, float fVolumedB)
    {
        CHECK_BOUNDS(nChannelId, sgpImplementation_->mChannels, );
        sgpImplementation_->mChannels[nChannelId]->setVolume(dBToVolume(fVolumedB));
    }

    bool FmodAudioEngine::isPlaying(int nChannelId) const
    {
        CHECK_BOUNDS(nChannelId, sgpImplementation_->mChannels, false);

        bool isPlaying = false;

        sgpImplementation_->mChannels[nChannelId]->isPlaying(&isPlaying);
        return isPlaying;
    }
}