#pragma once
#include <glm/glm.hpp>
#include "Utils.h"
#include "LevelGrid.h"
#include "Interface.h"

namespace dae
{
	class Scene;
	class PhysicsComponent;
	class GameObject;
	// --- More like blast cell ---
	class Blast final : public IICollideable
	{
	public:
		enum class Orientation
		{
			Central,
			Vertical,
			Horizontal
		};
		explicit Blast(Scene& scene, GridCell& cell, int size, Orientation orientation, dae::Utils::PlayerColors color);
		
		bool DoesCollide(const SDL_FRect& other) override;

		void Update();

	private:
		PhysicsComponent* m_PC{};
		GameObject* m_pGameObject{};
		bool m_HasExpired{ false };
		GridCell& m_HostCell;
		float m_Timer{ 0.5f };
	};
}