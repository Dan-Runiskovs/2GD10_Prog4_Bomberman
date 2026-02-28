#include "TransformComponent.h"
#include "GameObject.h"

dae::TransformComponent::TransformComponent(GameObject& owner) noexcept
	:TransformComponent(owner, glm::vec3{0.f, 0.f, 0.f})
{
}

dae::TransformComponent::TransformComponent(GameObject& owner, float x, float y, float z) noexcept
	:TransformComponent(owner, glm::vec3{x, y, z})
{
}

dae::TransformComponent::TransformComponent(GameObject& owner, glm::vec3 position) noexcept
	: ComponentBase(owner)
	, m_LocalPosition{ position }
{
}

const glm::vec3& dae::TransformComponent::GetWorldPosition()
{
	if (m_IsDirty) UpdateWorldPosition();

	return m_WorldPosition;
}

void dae::TransformComponent::SetWorldPosition(float x, float y, float z)
{
	SetWorldPosition(glm::vec3{ x, y, z });
}

void dae::TransformComponent::SetWorldPosition(const glm::vec3& newWorldPos)
{
	if (auto* pParent = GetOwner().GetParent())
	{
		const auto& parentWorld{ pParent->GetComponent<TransformComponent>().GetWorldPosition() };
		m_LocalPosition = newWorldPos - parentWorld;
	}
	else
	{
		m_LocalPosition = newWorldPos;
	}

	SetPositionDirty();
}
void dae::TransformComponent::SetLocalPosition(float x, float y, float z)
{
	m_LocalPosition.x = x;
	m_LocalPosition.y = y;
	m_LocalPosition.z = z;
	SetPositionDirty();
}

void dae::TransformComponent::SetLocalPosition(const glm::vec3& newLocalPos)
{
	m_LocalPosition = newLocalPos;
	SetPositionDirty();
}

void dae::TransformComponent::SetPositionDirty()
{
	m_IsDirty = true;

	for (unsigned int childIdx{ 0 }; childIdx < GetOwner().GetChildCount(); ++childIdx)
	{
		auto* pChild{ GetOwner().GetChildAt(childIdx) };
		pChild->GetComponent<TransformComponent>().SetPositionDirty();
	}
}

void dae::TransformComponent::UpdateWorldPosition()
{
	GameObject* pParent{ GetOwner().GetParent() };

	if (pParent)
	{
		auto& parentTransform{ pParent->GetComponent<TransformComponent>() };

		m_WorldPosition = parentTransform.GetWorldPosition() + m_LocalPosition;
	}
	else
	{
		m_WorldPosition = m_LocalPosition;
	}

	m_IsDirty = false;
}
