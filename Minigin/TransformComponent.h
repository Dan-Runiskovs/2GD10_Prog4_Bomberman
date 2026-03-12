#pragma once
#include <glm/glm.hpp>
#include "ComponentBase.h"

namespace dae
{
	class TransformComponent final : public ComponentBase
	{
	public:
		TransformComponent(GameObject& owner) noexcept;
		TransformComponent(GameObject& owner, float x, float y) noexcept;
		TransformComponent(GameObject& owner, glm::vec2 position) noexcept;

		void Update() override {}

		const glm::vec2& GetWorldPosition();
		void SetWorldPosition(float x, float y);
		void SetWorldPosition(const glm::vec2& newWorldPos);
		const glm::vec2& GetLocalPosition() const { return m_LocalPosition; };
		void SetLocalPosition(float x, float y);
		void SetLocalPosition(const glm::vec2& newLocalPos);

		void SetPositionDirty();
		
		~TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = delete;
		TransformComponent(TransformComponent&& other) = delete;
		TransformComponent& operator=(const TransformComponent& other) = delete;
		TransformComponent& operator=(TransformComponent&& other) = delete;
	private:
		glm::vec2 m_WorldPosition{ 0.f, 0.f };
		glm::vec2 m_LocalPosition{ 0.f, 0.f };

		bool m_IsDirty{ true };

		void UpdateWorldPosition();
	};
}