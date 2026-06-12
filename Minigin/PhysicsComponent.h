#pragma once
#include "ComponentBase.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <SDL3/SDL.h>

namespace dae
{
	class Scene;
	class TransformComponent;
	class PhysicsComponent final : public ComponentBase
	{
	public:
		PhysicsComponent(GameObject& owner, Scene& scene, const glm::vec2& dimensions, float speed = 100.f, bool isTrigger = false) noexcept;
		~PhysicsComponent();

		void Update() override {};

		void SetSpeed(float speed) { m_Speed = speed; }
		bool TryMove(const glm::vec2& dir, float searchRadius);
		const glm::vec2& GetOrigin() const;
		SDL_FRect GetBounds() const;
		void SetTrigger(bool newTriggerStatus) { m_IsTrigger = newTriggerStatus; }
		bool IsTrigger() const { return m_IsTrigger; }
		bool DoesIntersect(const PhysicsComponent& other) const;
		
		static bool DoesIntersect(const SDL_FRect& a, const SDL_FRect& b);

		PhysicsComponent(const PhysicsComponent& other) = delete;
		PhysicsComponent(PhysicsComponent&& other) = delete;
		PhysicsComponent& operator=(const PhysicsComponent& other) = delete;
		PhysicsComponent& operator=(PhysicsComponent&& other) = delete;
	private:
		Scene& m_Scene;
		float m_Speed{};
		bool m_IsTrigger{ false };
		glm::vec2 m_Dimensions{};
		TransformComponent& m_Transform;

		SDL_FRect GetBoundsAt(const glm::vec2& position) const;
	};
}