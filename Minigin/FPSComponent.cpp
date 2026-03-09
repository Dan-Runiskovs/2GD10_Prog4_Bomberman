#include "FPSComponent.h"
#include "GameObject.h"
#include "Timer.h"
#include <cmath>
#include <assert.h>
#include "TextComponent.h"

#include <iostream> // TODO: delete

dae::FPSComponent::FPSComponent(GameObject& owner) noexcept
	: ComponentBase(owner)
	, m_TextComponent{ owner.GetComponent<TextComponent>() }
{
}

void dae::FPSComponent::Update()
{
	const auto elapsed{ Timer::GetInstance().GetElapsed() };
	std::cout << "elapsed: " << elapsed << "!\n";
	m_InternalTimer += elapsed;
	++m_FrameCounter;
	std::cout << "FPS component frame: " << int(m_FrameCounter) << "\n";

	const float margin{ .5f };
	if (m_InternalTimer >= margin)
	{
		std::cout << "Internal timer : " << m_InternalTimer << "\n";
		std::cout << "frames passed since: " << int(m_FrameCounter) << "\n";
		float fps = m_FrameCounter / m_InternalTimer;
		fps = std::round(fps * 10) / 10;		// round to 1 decimal to avoid floating point errors
		
		std::cout << "Fps should be: " << fps << "\n";
		m_FrameCounter = 0;
		m_InternalTimer -= margin;
		std::string message = std::to_string(fps);
		message.resize(4);
		m_TextComponent.SetText(message + " FPS");
	}
}
