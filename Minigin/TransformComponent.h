#pragma once
#include <glm/glm.hpp>
#include "ComponentBase.h"

namespace dae
{
	class TransformComponent final : public ComponentBase
	{
	public:
		TransformComponent(GameObject& owner) noexcept;
		TransformComponent(GameObject& owner, float x, float y, float z = 0) noexcept;
		TransformComponent(GameObject& owner, glm::vec3 position) noexcept;

		void Update() override {}

		const glm::vec3& GetWorldPosition();
		void SetWorldPosition(float x, float y, float z = 0);
		void SetWorldPosition(const glm::vec3& newWorldPos);
		const glm::vec3& GetLocalPosition() const { return m_LocalPosition; };
		void SetLocalPosition(float x, float y, float z = 0);
		void SetLocalPosition(const glm::vec3& newLocalPos);

		void SetPositionDirty();
		
		virtual ~TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = delete;
		TransformComponent(TransformComponent&& other) = delete;
		TransformComponent& operator=(const TransformComponent& other) = delete;
		TransformComponent& operator=(TransformComponent&& other) = delete;
	private:
		glm::vec3 m_WorldPosition{ 0.f, 0.f, 0.f }; // Z will be used for drawing order high -> low ??
		glm::vec3 m_LocalPosition{ 0.f, 0.f, 0.f };

		bool m_IsDirty{ true };

		void UpdateWorldPosition();
	};
}