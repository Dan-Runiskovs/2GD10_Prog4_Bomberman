#include <algorithm>
#include <assert.h>
#include <glm/geometric.hpp>
#include "Scene.h"
#include "RenderComponent.h"
#include "PhysicsComponent.h"

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	m_objects.emplace_back(std::move(object));
}

void Scene::Remove(const GameObject& object)
{
	m_objects.erase(
		std::remove_if(
			m_objects.begin(),
			m_objects.end(),
			[&object](const auto& ptr) { return ptr.get() == &object; }
		),
		m_objects.end()
	);
}

void Scene::RemoveAll()
{
	m_objects.clear();
}

void Scene::Update()
{
	for(auto& object : m_objects)
	{
		object->Update();
	}
	CleanUpScene();
}

void Scene::CleanUpScene()
{
	std::erase_if(m_objects, [](const std::unique_ptr<GameObject>& obj) { return obj->IsMarkedForDelete(); });
}

std::vector<PhysicsComponent*> dae::Scene::GetNearbyPhysicalObjects(const glm::vec2& origin, float radius)
{
	std::vector<PhysicsComponent*> result;

	const float radiusSq{ radius * radius };

	for (auto* phys : m_PhysicalObjects)
	{
		const glm::vec2 delta
		{
			phys->GetOrigin() - origin
		};

		if (glm::dot(delta, delta) <= radiusSq)
		{
			result.push_back(phys);
		}
	}

	return result;
}

void Scene::Render() const
{
	for (const auto& object : m_objects)
	{
		auto* pRenderComponent = object->TryGetComponent<dae::RenderComponent>();
		if (pRenderComponent) pRenderComponent->Render();
	}
}

