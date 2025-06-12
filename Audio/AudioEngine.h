#include <string>

#include <glm/glm.hpp>

class AudioEngine
{
public:
    static void Init();
    static void Update();
    static void Shutdown();

    void LoadSound(const std::string& strSoundName,
        bool b3d=true, bool bLooping=false, bool bStream=false);
    void UnLoadSound(const std::string& strSoundName);
    int PlaySound(const std::string& strSoundName,
        const glm::vec3& vPos = glm::vec3{ 0,0,0 }, float fVolumedB = 0.0f);

    void Set3dListenerAndOrientation(const glm::vec3& vPosition,
        const glm::vec3& vLook, const glm::vec3& vUp);
    void StopChannel(int nChannelId);
    void StopAllChannels();
    void SetChannel3dPosition(int nChannelId, const glm::vec3& vPosition);
    void SetChannelVolume(int nChannelId, float fVolumedB);
    bool IsPlaying(int nChannelId) const;
};
