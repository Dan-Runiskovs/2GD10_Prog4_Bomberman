#pragma once
#include "ComponentBase.h"
#include <cstdint>
#include <glm/glm.hpp>

namespace dae
{
	class TransformComponent;
	class PhysicsComponent final : public ComponentBase
	{
	public:
		PhysicsComponent(GameObject& owner, const glm::vec2& mensions, float speed = 100.f) noexcept;
		~PhysicsComponent();

		void Update() override {};

		void SetSpeed(float speed) { m_Speed = speed; }
		//bool TryMove(int8_t x, int8_t y);
		bool TryMove(const glm::vec2& dir);

		PhysicsComponent(const PhysicsComponent& other) = delete;
		PhysicsComponent(PhysicsComponent&& other) = delete;
		PhysicsComponent& operator=(const PhysicsComponent& other) = delete;
		PhysicsComponent& operator=(PhysicsComponent&& other) = delete;
	private:
		float m_Speed{};
		glm::vec2 m_Dimensions{};
		TransformComponent& m_Transform;
	};
}