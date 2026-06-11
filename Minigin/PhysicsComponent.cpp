#include "PhysicsComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Timer.h"
#include <glm/geometric.hpp>
#include <limits>

dae::PhysicsComponent::PhysicsComponent(GameObject& owner, const glm::vec2& dimensions, float speed) noexcept
	: ComponentBase(owner)
	, m_Speed{ speed }
    , m_Dimensions{ dimensions }
	, m_Transform{ owner.GetComponent<TransformComponent>() }
{
}

dae::PhysicsComponent::~PhysicsComponent()
{
    // Unsubscribe oneself from global list
}

bool dae::PhysicsComponent::TryMove(const glm::vec2& dir)
{
    /*
    @DEV pass only physics component references of physics objects 
    that exist only: south, north, east, west (they always do) :)

    if collides, don't move, else - move

    PhysicsComponent* (&myArray)[8] collect array of pointers
    Scene can yield a std::vector<PhysicsComponent*>
    set limitRadius
    */

    const auto elapsedSec{ Timer::GetInstance().GetElapsed() };

    const auto direction{ glm::normalize(dir) };

    const auto& currentPos{ m_Transform.GetLocalPosition() };

    m_Transform.SetLocalPosition(
        currentPos + direction * elapsedSec);

    return true;
}
