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