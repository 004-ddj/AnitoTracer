#include "AudioSourceComponent.hpp"

void AudioSourceComponent::Play() {
    if (!HasAudioClip()) return;
    if (m_soundID != 0) AudioManager::GetInstance().StopClip(m_soundID);;
    m_soundID = AudioManager::GetInstance().PlayClip(m_audioClip.Get());
}

void AudioSourceComponent::Stop() {
    if (m_soundID == 0) return;
    AudioManager::GetInstance().StopClip(m_soundID);
    m_soundID = 0;
}