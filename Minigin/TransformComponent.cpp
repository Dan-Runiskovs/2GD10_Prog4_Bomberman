#include "TransformComponent.h"

dae::TransformComponent::TransformComponent(GameObject& owner) noexcept
	:TransformComponent(owner, glm::vec3{0.f, 0.f, 0.f})
{
}

dae::TransformComponent::TransformComponent(GameObject& owner, float x, float y, float z) noexcept
	:TransformComponent(owner, glm::vec3{x, y, z})
{
}

dae::TransformComponent::TransformComponent(GameObject& owner, glm::vec3 position) noexcept
	: ComponentBase(owner, ID)
	, m_Position{ position }
{
}

void dae::TransformComponent::SetPosition(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void dae::TransformComponent::SetPosition(glm::vec3 position)
{
	m_Position = position;
}
