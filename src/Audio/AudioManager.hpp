#pragma once

#include "AudioClip.hpp"
#include "AssetLoading/AssetLoader.hpp"

class AudioManager : public gbe::AssetLoader<AudioClip> {
public:
    static AudioManager& GetInstance() {
        static AudioManager instance;
        instance.AssignSelfAsLoader();
        return instance;
    }

    // Returns a pointer to the cached clip, or loads it if not present
    AudioClip* LoadClip(const std::string& filepath);

    // Clears the cache
    void ClearCache();

private:
    AudioManager() = default;
    ~AudioManager() { ClearCache(); }
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    std::unordered_map<std::string, std::unique_ptr<AudioClip>> m_AudioCache;
};