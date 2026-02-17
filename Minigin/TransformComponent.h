#pragma once
#include <glm/glm.hpp>
#include "ComponentBase.h"

namespace dae
{
	class TransformComponent final : public ComponentBase
	{
	public:
		TransformComponent(std::shared_ptr<GameObject> pOwner, float x, float y, float z = 0);
		TransformComponent(std::shared_ptr<GameObject> pOwner, glm::vec3 position);

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(float x, float y, float z = 0);
		void SetPosition(glm::vec3 position);
		
		virtual ~TransformComponent() = default;

		TransformComponent(const TransformComponent& other) = delete;
		TransformComponent(TransformComponent&& other) = delete;
		TransformComponent& operator=(const TransformComponent& other) = delete;
		TransformComponent& operator=(TransformComponent&& other) = delete;
	private:
		glm::vec3 m_Position; // Z will be used for drawing order high -> low
	};
}