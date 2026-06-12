#include "PhysicsComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Timer.h"
#include "Scene.h"
#include <glm/geometric.hpp>
#include <limits>
#include <algorithm>
#include <iostream>

dae::PhysicsComponent::PhysicsComponent(GameObject& owner, Scene& scene, const glm::vec2& dimensions, float speed, bool isTrigger) noexcept
	: ComponentBase(owner)
    , m_Scene{ scene }
	, m_Speed{ speed }
    , m_Dimensions{ dimensions }
	, m_Transform{ owner.GetComponent<TransformComponent>() }
    , m_IsTrigger{ isTrigger }
{
    // --- Register oneself in the scene's physical object storage ---
    m_Scene.GetPhysicalObjects().push_back(this);
}

dae::PhysicsComponent::~PhysicsComponent()
{
    auto& objects = m_Scene.GetPhysicalObjects();

    auto it = std::find(objects.begin(), objects.end(), this);

    if (it != objects.end()) objects.erase(it);
}

bool dae::PhysicsComponent::TryMove(const glm::vec2& dir, float searchRadius)
{
    /*
    @DEV pass only physics component references of physics objects 
    that exist only: south, north, east, west (they always do) :)

    if collides, don't move, else - move

    PhysicsComponent* (&myArray)[8] collect array of pointers
    Scene can yield a std::vector<PhysicsComponent*>
    set limitRadius
    */

    std::cout << "Trying to move!\n";
    const auto elapsedSec{ Timer::GetInstance().GetElapsed() };

    // --- Safety + direction ---
    if (glm::dot(dir, dir) <= std::numeric_limits<float>::epsilon()) return false;
    const auto direction{ glm::normalize(dir) };

    const auto& currentPos{ m_Transform.GetLocalPosition() };

    // --- Calculate Next ---
    const glm::vec2 nextPos{ currentPos + direction * m_Speed * elapsedSec };
    const SDL_FRect nextBounds{ GetBoundsAt(nextPos) };

    // --- Check AABB collisions ---
    for(auto* other : m_Scene.GetNearbyPhysicalObjects(this->GetOrigin(), searchRadius))
    {
        if (other == this) continue;

        if (DoesIntersect(nextBounds, other->GetBounds()))
        {
            if (!other->IsTrigger()) return false;

            // Callback to TriggerEvent?
        }
    }

    m_Transform.SetLocalPosition(nextPos);

    return true;
}

const glm::vec2& dae::PhysicsComponent::GetOrigin() const
{
    return m_Transform.GetWorldPosition();
}

SDL_FRect dae::PhysicsComponent::GetBounds() const
{
    // --- ASSUMING Origin == centre ---
    const auto& pos{ GetOrigin() };

    return SDL_FRect
    {
        pos.x - m_Dimensions.x * 0.5f,
        pos.y - m_Dimensions.y * 0.5f,
        m_Dimensions.x,
        m_Dimensions.y
    };
}

bool dae::PhysicsComponent::DoesIntersect(const SDL_FRect& a, const SDL_FRect& b)
{
    return
        a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y;
}

SDL_FRect dae::PhysicsComponent::GetBoundsAt(const glm::vec2& position) const
{
    return SDL_FRect
    {
        position.x - m_Dimensions.x * 0.5f,
        position.y - m_Dimensions.y * 0.5f,
        m_Dimensions.x,
        m_Dimensions.y
    };
}
