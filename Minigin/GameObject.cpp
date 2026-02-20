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
	for (auto const& pComponent : m_pComponents)
	{
		pComponent->Update();
	}
}