#pragma once

#include <iostream>
#include "AudioClip.hpp"
#include "AssetLoading/AssetLoader.hpp"
#include "miniaudio.h"

class AudioManager : public gbe::AssetLoader<AudioClip> {
public:
    static AudioManager& GetInstance() {
        static AudioManager instance;
        instance.AssignSelfAsLoader();
        return instance;
    }

    // Returns a pointer to the cached clip, or loads it if not present
    AudioClip* LoadClip(const std::string& filepath);

    // Plays a given clip
    uint64_t PlayClip(AudioClip* audioClip);

    // Stop a currently playing sound
    void StopClip(uint64_t soundID);

    // Clears the cache
    void ClearCache();

private:
    AudioManager() {
        ma_result result = ma_engine_init(nullptr, &m_AudioEngine);
        if (result != MA_SUCCESS) {
            std::cerr << "Failed to load audio engine." << std::endl;
        } else {
            m_AudioEngineInitialized = true;
        }
    }
    ~AudioManager() { ClearCache(); }
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;  

    struct AudioSoundDeleter {
        void operator()(ma_sound* sound) const;
    };

    ma_engine m_AudioEngine;
    bool m_AudioEngineInitialized = false;
    uint64_t m_NextSoundID = 1;
    std::unordered_map<uint64_t, std::unique_ptr<ma_sound, AudioSoundDeleter>> m_PlayingSounds;

    std::unordered_map<std::string, std::unique_ptr<AudioClip>> m_AudioCache;

    void ClearClips();
};