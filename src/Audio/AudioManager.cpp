#include "AudioManager.hpp"

AudioClip* AudioManager::LoadClip(const std::string& filepath) {
    // Check & return cache
    auto it = m_AudioCache.find(filepath);
    if (it != m_AudioCache.end()) {
        return it->second.get();
    }

    auto pAudioClip = std::make_unique<AudioClip>();

    AudioClip* rawPtr = pAudioClip.get();
    rawPtr->SetPath(filepath);

    // Optimized cache insertion
    m_AudioCache.emplace(filepath, std::move(pAudioClip));

    return rawPtr;
}

//  Custom deleter for m_PlayingSounds; release miniaudio's internal
//  resources before freeing the ma_sound itself.   
void AudioManager::AudioSoundDeleter::operator()(ma_sound* sound) const {
    ma_sound_uninit(sound);
    delete sound;
}

uint64_t AudioManager::PlayClip (AudioClip* audioClip) {
    // Return if audio engine never initialized
    if (!m_AudioEngineInitialized) {
        std::cerr << "Failed to play clip: audio engine not initialized." << std::endl;
        return 0;
    }

    if (!audioClip) {
        std::cerr << "No audio found." << std::endl;
        return 0;
    }

    // Get the file path from audioClip
    std::filesystem::path path = audioClip->GetPath();

    // Alloc + init new ma_sound from that path
    ma_sound* newSound = new ma_sound;
    ma_result soundResult;

    soundResult = ma_sound_init_from_file(&m_AudioEngine, path.string().c_str(), 0, nullptr, nullptr, newSound);

    // Check if init succeeded
    if (soundResult != MA_SUCCESS){
        std::cerr << "Failed to load sound." << std::endl;
        delete newSound;
        return 0;
    }

    // Hand the newSound off to m_pAudioSound & start playback
    uint64_t newID = m_NextSoundID++;

    m_PlayingSounds[newID] = std::unique_ptr<ma_sound, AudioSoundDeleter>(newSound);
    ma_sound_start(m_PlayingSounds[newID].get());

    return newID;
}

void AudioManager::StopClip (uint64_t soundID) {
    // Return if audio engine never initialized
    if (!m_AudioEngineInitialized) {
        std::cerr << "Failed to stop clip: audio engine not initialized." << std::endl;
        return;
    }
    
    // Return if there is no audio to stop
    auto it = m_PlayingSounds.find(soundID);
    if (it == m_PlayingSounds.end()) {
        std::cerr << "Audio not found: stop clip failed. ID: " << soundID << std::endl;
        return;
    }

    // Stop playback without destroying sound
    ma_sound_stop(it->second.get());
    m_PlayingSounds.erase(it);
}

void AudioManager::SetSoundPosition(uint64_t soundID, glm::vec3 position) {
    // Return if audio engine never initialized
    if (!m_AudioEngineInitialized) {
        std::cerr << "Failed to set sound position: audio engine not initialized." << std::endl;
        return;
    }

    // Return if there is no sound to set position
    auto it = m_PlayingSounds.find(soundID);
    if (it == m_PlayingSounds.end()) {
        std::cerr << "Audio not found: set sound position failed. ID: " << soundID << std::endl;
        return;
    }

    // Set sound position
    ma_sound_set_position(it->second.get(), position.x, position.y, position.z);
}

void AudioManager::SetListenerPosition(glm::vec3 position) {
    // Return if audio engine never initialized
    if (!m_AudioEngineInitialized) {
        std::cerr << "Failed to set listener position: audio engine not initialized." << std::endl;
        return;
    }

    // Set engine position
    ma_engine_listener_set_position(&m_AudioEngine, 0, position.x, position.y, position.z);
}

void AudioManager::SetListenerDirection(glm::vec3 direction) {
    // Return if audio engine never initialized
    if (!m_AudioEngineInitialized) {
        std::cerr << "Failed to set listener direction: audio engine not initialized." << std::endl;
        return;
    }

    // Set engine direction
    ma_engine_listener_set_direction(&m_AudioEngine, 0, direction.x, direction.y, direction.z);
}

void AudioManager::ClearCache() {
    m_AudioCache.clear();
    ClearClips();
}

void AudioManager::ClearClips() {
    m_PlayingSounds.clear();
}