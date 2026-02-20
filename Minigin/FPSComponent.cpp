#include "FPSComponent.h"
#include "GameObject.h"
#include "Timer.h"
#include <cmath>
#include <assert.h>
#include "TextComponent.h"

dae::FPSComponent::FPSComponent(GameObject& owner) noexcept
	: ComponentBase(owner)
	, m_TextComponent{ owner.GetComponent<TextComponent>() }
{
}

void dae::FPSComponent::Update()
{
	m_InternalTimer += Timer::GetInstance().GetElapsed();
	++m_FrameCounter;

	const float margin{ 1.f };
	if (m_InternalTimer >= margin)
	{
		float fps = m_FrameCounter / m_InternalTimer;
		fps = std::round(fps * 10) / 10;		// round to 1 decimal to avoid floating point errors
		m_FrameCounter = 0;
		m_InternalTimer -= margin;
		std::string message = std::to_string(fps);
		message.resize(4);
		m_TextComponent.SetText(message + " FPS");
	}
}
