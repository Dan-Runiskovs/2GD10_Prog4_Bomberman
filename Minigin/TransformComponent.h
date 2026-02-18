#pragma once
#include <glm/glm.hpp>
#include "ComponentBase.h"

namespace dae
{
	class TransformComponent final : public ComponentBase
	{
	public:
		static constexpr ComponentID ID{ ComponentID::Transform };

		TransformComponent(GameObject& owner) noexcept;
		TransformComponent(GameObject& owner, float x, float y, float z = 0) noexcept;
		TransformComponent(GameObject& owner, glm::vec3 position) noexcept;

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(float x, float y, float z = 0);
		void SetPosition(glm::vec3 position);
		
		virtual ~TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = delete;
		TransformComponent(TransformComponent&& other) = delete;
		TransformComponent& operator=(const TransformComponent& other) = delete;
		TransformComponent& operator=(TransformComponent&& other) = delete;
	private:
		glm::vec3 m_Position; // Z will be used for drawing order high -> low ??
	};
}