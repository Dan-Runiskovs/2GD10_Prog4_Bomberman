#include "TransformComponent.h"
#include "GameObject.h"

dae::TransformComponent::TransformComponent(GameObject& owner) noexcept
	:TransformComponent(owner, glm::vec2{0.f, 0.f})
{
}

dae::TransformComponent::TransformComponent(GameObject& owner, float x, float y) noexcept
	:TransformComponent(owner, glm::vec2{x, y})
{
}

dae::TransformComponent::TransformComponent(GameObject& owner, glm::vec2 position) noexcept
	: ComponentBase(owner)
	, m_LocalPosition{ position }
{
}

const glm::vec2& dae::TransformComponent::GetWorldPosition()
{
	if (m_IsDirty) UpdateWorldPosition();

	return m_WorldPosition;
}

void dae::TransformComponent::SetWorldPosition(float x, float y)
{
	SetWorldPosition(glm::vec2{ x, y });
}

void dae::TransformComponent::SetWorldPosition(const glm::vec2& newWorldPos)
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
void dae::TransformComponent::SetLocalPosition(float x, float y)
{
	m_LocalPosition.x = x;
	m_LocalPosition.y = y;
	SetPositionDirty();
}

void dae::TransformComponent::SetLocalPosition(const glm::vec2& newLocalPos)
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
