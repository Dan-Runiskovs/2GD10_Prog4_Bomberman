#include "OrbitComponent.h"
#include "GameObject.h"
#include "Timer.h"
#include "TransformComponent.h"

dae::OrbitComponent::OrbitComponent(GameObject& owner)
	: OrbitComponent(owner, 100.f, float(PI))
{
}

dae::OrbitComponent::OrbitComponent(GameObject& owner, float radius)
	: OrbitComponent(owner, radius, float(PI))
{
}

dae::OrbitComponent::OrbitComponent(GameObject& owner, float radius, float rotSpeed = float(PI))
	: ComponentBase(owner)
	, m_Radius{radius}
	, m_RotationSpeed{rotSpeed}
	, m_TransformComponent{ owner.GetComponent<TransformComponent>() }
{
}

void dae::OrbitComponent::Update()
{
	auto* pParent = GetOwner().GetParent();
	if (!pParent) return;						// No parent -> no rotation

	if (!m_IsRotating) return;					// No rotation 0_0

	constexpr float nearZero{ 1e-3f };
	if (m_Radius <= nearZero) return;			// No radius -> no rotation
	if (m_RotationSpeed <= nearZero) return;	// No speed -> no rotation

	//calculate current angle
	constexpr float twoPi{ float(PI) * 2 };
	const float delta{ Timer::GetInstance().GetElapsed() };
	const float direction{ m_IsRotatingClockwise ? -1.f : 1.f };

	m_CurrentAngle += direction * m_RotationSpeed * delta;
	m_CurrentAngle = fmod(m_CurrentAngle, twoPi);
	if (m_CurrentAngle < 0.f) m_CurrentAngle += twoPi;

	//calculate newX and newY
	const float newX{ cosf(m_CurrentAngle) * m_Radius };
	const float newY{ sinf(m_CurrentAngle) * m_Radius };

	//update local position
	m_TransformComponent.SetLocalPosition(newX, newY);
}
