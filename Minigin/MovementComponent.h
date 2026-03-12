#pragma once
#include "ComponentBase.h"
#include <cstdint>

namespace dae
{
	class TransformComponent;
	class PhysicsComponent final : public ComponentBase
	{
	public:
		PhysicsComponent(GameObject& owner, float speed = 100.f) noexcept;

		void Update() override {};

		void SetSpeed(float speed) { m_Speed = speed; }
		bool TryMove(int8_t x, int8_t y);

		~PhysicsComponent() = default;
		PhysicsComponent(const PhysicsComponent& other) = delete;
		PhysicsComponent(PhysicsComponent&& other) = delete;
		PhysicsComponent& operator=(const PhysicsComponent& other) = delete;
		PhysicsComponent& operator=(PhysicsComponent&& other) = delete;
	private:
		float m_Speed{};
		TransformComponent& m_Transform;
	};
}