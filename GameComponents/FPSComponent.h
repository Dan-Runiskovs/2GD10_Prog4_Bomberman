#pragma once
#include "ComponentBase.h"

namespace dae
{
	class TextComponent;
	class FPSComponent final : public ComponentBase
	{
	public:
		FPSComponent(GameObject& owner) noexcept;
		void Update() override;

		~FPSComponent() = default;

		FPSComponent(const FPSComponent& other) noexcept = delete;
		FPSComponent(FPSComponent&& other) noexcept = delete;
		FPSComponent& operator=(const FPSComponent& other) noexcept = delete;
		FPSComponent& operator=(FPSComponent&& other) noexcept = delete;
	private:
		float m_InternalTimer{};
		unsigned char m_FrameCounter{};

		TextComponent& m_TextComponent; // Requires Text Component
	};
}