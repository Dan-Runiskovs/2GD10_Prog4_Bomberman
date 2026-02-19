#include <algorithm>
#include <assert.h>
#include "Scene.h"
#include "RenderComponent.h"

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
}

void Scene::CleanUpScene()
{
	for (auto& object : m_objects)
	{
		if (object->IsMarkedForDelete())
			m_objectsToDelete.emplace_back(std::move(object));
	}
}

void Scene::Render() const
{
	for (const auto& object : m_objects)
	{
		object->TryGetComponent<dae::RenderComponent>()->Render();
	}
}

