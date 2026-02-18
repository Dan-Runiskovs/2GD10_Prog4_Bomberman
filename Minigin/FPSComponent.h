#pragma once
#include "ComponentBase.h"

namespace dae
{
	class TextComponent;
	class FPSComponent final : public ComponentBase
	{
	public:
		static constexpr ComponentID ID{ ComponentID::FPS };

		FPSComponent(GameObject& owner) noexcept;
		void Update() override;

		const float GetFPS() const { return m_FPS; }

		~FPSComponent() = default;

		FPSComponent(const FPSComponent& other) noexcept = delete;
		FPSComponent(FPSComponent&& other) noexcept = delete;
		FPSComponent& operator=(const FPSComponent& other) noexcept = delete;
		FPSComponent& operator=(FPSComponent&& other) noexcept = delete;
	private:
		float m_FPS{};
		float m_InternalTimer{};
		unsigned char m_FrameCounter{};

		TextComponent* m_pTextComponent{ nullptr }; // Requires Text Component
	};
}