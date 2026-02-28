#include <string>
#include <algorithm>
#include "GameObject.h"
#include "ResourceManager.h"
#include "TransformComponent.h"
#include "Renderer.h"

dae::GameObject::GameObject()
	: GameObject(nullptr)
{
}

dae::GameObject::GameObject(GameObject* pParent)
	: m_pParent{ pParent }
{
	m_pComponents.reserve(5);
	AddComponent<TransformComponent>();		//Existing without transform makes no sense
}

void dae::GameObject::Update() const
{
	for (auto const& pComponent : m_pComponents)  pComponent->Update(); 
}

void dae::GameObject::MarkForDelete()
{
	m_IsMarkedForDelete = true;

	for (auto& pChild : m_pChildren)
	{
		pChild->MarkForDelete();
	}
}

bool dae::GameObject::IsChild(GameObject* parent) const
{
	GameObject* pCurrentParent = m_pParent;

	while (pCurrentParent)
	{
		if (pCurrentParent == parent)
			return true;
		// Can go up to grandparents and beyond
		pCurrentParent = pCurrentParent->m_pParent;
	}
	//fails if there is no parent or is looking in another tree
	return false;
}

void dae::GameObject::SetParent(GameObject* parent, bool keepWorldPosition)
{
	TransformComponent& transform = GetComponent<TransformComponent>();
	if (IsChild(parent) || parent == this || m_pParent == parent)
		return;
	if (parent == nullptr)
		transform.SetLocalPosition(transform.GetWorldPosition());
	else
	{
		if (keepWorldPosition)
			transform.SetLocalPosition(transform.GetWorldPosition() - parent->GetComponent<TransformComponent>().GetWorldPosition());
		transform.SetPositionDirty();
	}
	if (m_pParent) m_pParent->RemoveChild(this);
	m_pParent = parent;
	if (m_pParent) m_pParent->AddChild(this);
}

unsigned int dae::GameObject::GetChildCount() const
{
	return static_cast<unsigned int>(m_pChildren.size());
}

void dae::GameObject::AddChild(GameObject* pChild)
{
	if (!pChild) return;

	if (std::ranges::find(m_pChildren, pChild) == m_pChildren.end())
	{
		//TODO: Remove oneself from previous parent
		m_pChildren.emplace_back(pChild);
	}
}

void dae::GameObject::RemoveChild(GameObject* pChild)
{
	if (auto it = std::ranges::find(m_pChildren, pChild); it != m_pChildren.end())
	{
		m_pChildren.erase(it);
	}
}
