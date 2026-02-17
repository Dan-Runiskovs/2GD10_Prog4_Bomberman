#include "FPSComponent.h"
#include "GameObject.h"
#include "Timer.h"

dae::FPSComponent::FPSComponent(std::shared_ptr<GameObject> pOwner)
	: ComponentBase(ComponentID::FPS, std::move(pOwner))
{
}

void dae::FPSComponent::Update()
{
	m_InternalTimer += Timer::GetElapsed();
	++m_FrameCounter;

	const float margin{ 0.25f };
	if (m_InternalTimer >= margin)
	{
		m_FPS = m_FrameCounter / m_InternalTimer;
		m_FrameCounter = 0;
		m_InternalTimer -= margin;
	}
}
