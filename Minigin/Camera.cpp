#include "Camera.h"
#include "Renderer.h"
#include <algorithm>

void dae::Camera::Aim(const glm::vec2& target, float levelWidth, float levelHeight)
{
    const auto windowSize{ Renderer::GetInstance().GetWindowSize() };
    m_Position.x = target.x - windowSize.x * 0.5f;

    m_Position.y = target.y - windowSize.y * 0.5f;

    m_Position.x = std::clamp(m_Position.x, 0.f, std::max(0.f, levelWidth - windowSize.x));

    m_Position.y = std::clamp(m_Position.y, 0.f, std::max(0.f, levelHeight - windowSize.y));
}

void dae::Camera::Reset()
{
    m_Position = {};
}
