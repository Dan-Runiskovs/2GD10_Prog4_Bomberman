#include "MovementComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Timer.h"

dae::PhysicsComponent::PhysicsComponent(GameObject& owner, float speed) noexcept
	: ComponentBase(owner)
	, m_Speed{ speed }
	, m_Transform{ owner.GetComponent<TransformComponent>() }
{
}

bool dae::PhysicsComponent::TryMove(int8_t x, int8_t y)
{
    const auto elapsedSec{ Timer::GetInstance().GetElapsed() };

    const float moveX{ x * m_Speed * elapsedSec };
    const float moveY{ y * m_Speed * elapsedSec };

    const auto& currentPos{ m_Transform.GetLocalPosition() };

    m_Transform.SetLocalPosition(
        currentPos.x + moveX,
        currentPos.y + moveY );

    return true;
}
