#include "FPSComponent.h"
#include "GameObject.h"
#include "Timer.h"
#include <cmath>
#include <format>
#include <assert.h>
#include "TextComponent.h"

dae::FPSComponent::FPSComponent(GameObject& owner) noexcept
	:ComponentBase(owner, ID)
{
	assert(owner.HasComponent(ComponentID::Text) && "Owner must have text component!");
}

void dae::FPSComponent::Update()
{
	m_InternalTimer += Timer::GetInstance().GetElapsed();
	++m_FrameCounter;

	const float margin{ 0.25f };
	if (m_InternalTimer >= margin)
	{
		m_FPS = m_FrameCounter / m_InternalTimer;
		m_FPS = std::round(m_FPS * 10) / 10;		// round to 1 decimal to avoid floating point errors
		m_FrameCounter = 0;
		m_InternalTimer -= margin;
		m_Owner.TryGetComponent<TextComponent>()->SetText(std::format("{:.1f} FPS", m_FPS));
	}
}
