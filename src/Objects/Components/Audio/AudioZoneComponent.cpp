#include "AudioZoneComponent.hpp"

void AudioZoneComponent::OnUpdate(float deltaTime) {
    (void)deltaTime;

    // Get Transform from component's owner
    HierarchyObject* owner = GetOwner().GetPtr();
    if (!owner) return;

    Transform* audioZoneTransform = owner->GetTransform();
    if (!audioZoneTransform) return;

    // Get position from Transform
    const glm::vec3 audioZonePos = audioZoneTransform->GetPosition();
    const glm::vec3 audioListenPos = AudioManager::GetInstance().GetListenerPosition();

    float distance = glm::distance(audioZonePos, audioListenPos);

    static int frameCounter = 0;
    if (++frameCounter % 120 == 1) 
        std::cout << "[AudioZone] update pos=(" << audioZonePos.x << "," << audioZonePos.y << ","
            << audioZonePos.z << ") distance=(" << distance << ")" << std::endl;

    const uint64_t fadeLength = GetFadeLength();
    float enterRadius = std::clamp(m_enterRadius, 0.0f, 1000.0f);
    float exitMargin = std::clamp(m_exitMargin, 0.0f, 500.0f);

    if (m_audioClip.IsEmpty()) return;

    if (!m_isInsideZone && distance < enterRadius) {
        m_activeKey = MakeOwnerKey();
        std::cout << "[AudioZone] push key=" << m_activeKey << std::endl;
        AudioManager::GetInstance().PushMusic(m_activeKey, m_audioClip.Get(), fadeLength);
        m_isInsideZone = true;
    } else if (m_isInsideZone && distance > (enterRadius + exitMargin)) {
        AudioManager::GetInstance().PopMusic(m_activeKey, fadeLength);
        m_isInsideZone = false;
    }
}