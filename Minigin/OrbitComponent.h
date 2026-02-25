#pragma once
#include <glm/glm.hpp>
#include "ComponentBase.h"

constexpr double PI = 3.14159265358979323846;

namespace dae
{
	class OrbitComponent final : public ComponentBase
	{
	public :
		OrbitComponent(GameObject& owner);
		OrbitComponent(GameObject& owner, float radius);
		OrbitComponent(GameObject& owner, float radius, float rotSpeed);

		void Update();

		void FlipDirection() { m_IsRotatingClockwise = !m_IsRotatingClockwise; }
		void SetRadius(float newRadius) { m_Radius = newRadius; }
		void SetRotationSpeed(float newSpeed) { m_RotationSpeed = newSpeed; }

	private:
		bool m_IsRotating{ true };
		bool m_IsRotatingClockwise{ true };
		float m_Radius{};					// Radius at which we are located from parent object
		float m_CurrentAngle{0.f};			// Current angle in Rad
		float m_RotationSpeed{};
	};
}