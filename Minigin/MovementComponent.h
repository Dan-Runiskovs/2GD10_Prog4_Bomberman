#pragma once
#include "ComponentBase.h"
#include <cstdint>

namespace dae
{
	class TransformComponent;
	class MovementComponent final : public ComponentBase
	{
	public:
		MovementComponent(GameObject& owner, float speed = 100.f) noexcept;

		void Update() override {};

		void SetSpeed(float speed) { m_Speed = speed; }
		bool TryMove(int8_t x, int8_t y);

		~MovementComponent() = default;
		MovementComponent(const MovementComponent& other) = delete;
		MovementComponent(MovementComponent&& other) = delete;
		MovementComponent& operator=(const MovementComponent& other) = delete;
		MovementComponent& operator=(MovementComponent&& other) = delete;
	private:
		float m_Speed{};
		TransformComponent& m_Transform;
	};
}