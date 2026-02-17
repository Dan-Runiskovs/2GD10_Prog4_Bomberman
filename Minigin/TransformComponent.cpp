#include "TransformComponent.h"

dae::TransformComponent::TransformComponent(std::shared_ptr<GameObject> pOwner, float x, float y, float z)
	: TransformComponent(std::move(pOwner), glm::vec3{x, y, z})
{
}

dae::TransformComponent::TransformComponent(std::shared_ptr<GameObject> pOwner, glm::vec3 position)
	: ComponentBase(ComponentID::Transform, std::move(pOwner))
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
