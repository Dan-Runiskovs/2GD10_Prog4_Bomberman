#pragma once
#include "ComponentBase.h"

namespace dae
{
	class FPSComponent final : public ComponentBase
	{
	public:
		FPSComponent(std::shared_ptr<GameObject> pOwner);
		void Update() override;

		const float GetFPS() const { return m_FPS; }

		~FPSComponent() = default;

		FPSComponent(const FPSComponent& other) = delete;
		FPSComponent(FPSComponent&& other) = delete;
		FPSComponent& operator=(const FPSComponent& other) = delete;
		FPSComponent& operator=(FPSComponent&& other) = delete;
	private:
		float m_FPS{};
		float m_InternalTimer{};
		unsigned char m_FrameCounter{};
	};
}