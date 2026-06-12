#pragma once
#include <glm/glm.hpp>
#include "Utils.h"
#include "LevelGrid.h"

namespace dae
{
	class Scene;
	class PhysicsComponent;
	class GameObject;
	// --- More like blast cell ---
	class Blast final
	{
	public:
		enum class Orientation
		{
			Central,
			Vertical,
			Horizontal
		};
		explicit Blast(Scene& scene, GridCell& cell, int size, Orientation orientation, dae::Utils::PlayerColors color);
		~Blast();

		void Update();

	private:
		PhysicsComponent* m_PC{};
		GameObject* m_pGameObject{};
		bool m_HasExpired{ false };
		GridCell& m_HostCell;
		float m_Timer{ 0.5f };
	};
}