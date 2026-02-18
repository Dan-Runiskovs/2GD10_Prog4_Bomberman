#include <string>
#include <algorithm>
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"

void dae::GameObject::Update() const
{
	for (auto const& pComponent : m_pComponents)
	{
		pComponent->Update();
	}
}

std::optional<std::vector<std::unique_ptr<dae::ComponentBase>>::iterator> dae::GameObject::HasComponent(ComponentBase::ComponentID id)
{
	for (auto it = m_pComponents.begin(); it != m_pComponents.end(); ++it)
	{
		if ((*it)->HasSameID(id))
			return it;
	}
	return std::nullopt;
}
