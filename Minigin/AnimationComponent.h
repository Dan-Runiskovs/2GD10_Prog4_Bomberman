#pragma once
#include "ComponentBase.h"
#include <vector>
#include <cstdint>

namespace dae
{
	class RenderComponent;

	struct Animation
	{
		uint8_t startFrame{};
		uint8_t frameCount{};
		bool loop{ true };
		float frameTime{ 0.2f };
	};

	class AnimationComponent final : public ComponentBase
	{
	public:
		AnimationComponent(GameObject& owner, uint8_t frameWidth, uint8_t frameHeight, uint8_t columns) noexcept;

		void Update() override;

		void AddAnimation(const Animation& animation);
		void PlayAnimation(size_t animationIdx);

		void Play() { m_Playing = true; }
		void Pause() { m_Playing = false; }
		bool IsPlaying() const { return m_Playing; }

	private:
		RenderComponent& m_RenderComponent;

		const uint8_t m_FrameWidth;
		const uint8_t m_FrameHeight;
		const uint8_t m_Columns;

		std::vector<Animation> m_Animations;

		size_t m_CurrentAnimation{};
		uint8_t m_CurrentFrame{};

		float m_Timer{};
		bool m_Playing{ true };
	};
}