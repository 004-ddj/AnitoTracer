#include "AudioManager.hpp"

AudioClip* AudioManager::LoadClip(const std::string& filepath) {
    // Check cache
    auto it = m_AudioCache.find(filepath);
    if (it != m_AudioCache.end()) {
        return it->second.get();
    }

    auto pAudioClip = std::make_unique<AudioClip>();

    AudioClip* rawPtr = pAudioClip.get();

    // Optimized cache insertion
    m_AudioCache.emplace(filepath, std::move(pAudioClip));

    return rawPtr;
}

void AudioManager::ClearCache() {
    m_AudioCache.clear();
}