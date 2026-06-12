#pragma once
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "GameObject.h"

namespace dae
{
	class PhysicsComponent;
	class Scene final
	{
	public:
		void Add(std::unique_ptr<GameObject> object);
		void Remove(const GameObject& object);
		void RemoveAll();

		void Update();
		void Render() const;
		void CleanUpScene();

		std::vector<PhysicsComponent*>& GetPhysicalObjects() { return m_PhysicalObjects; }
		std::vector<PhysicsComponent*> GetNearbyPhysicalObjects(const glm::vec2& origin, float radius);

		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private:
		friend class SceneManager;
		explicit Scene() = default;

		std::vector <std::unique_ptr<GameObject>> m_objects{};
		std::vector <std::unique_ptr<GameObject>> m_objectsToDelete{};
		std::vector <PhysicsComponent*> m_PhysicalObjects;
	};

}
