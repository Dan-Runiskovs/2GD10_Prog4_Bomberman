#include "Timer.h"

void dae::Timer::Start()
{
	m_IsRunning = true;
	m_CurrentTime = std::chrono::steady_clock::now();
}

void dae::Timer::Stop()
{
	m_IsRunning = false;
}

float dae::Timer::GetElapsed() const
{
	std::chrono::duration<float> delta = m_CurrentTime - m_PreviousTime;

	return delta.count();
}

void dae::Timer::Update()
{
	if (!m_IsRunning) return;

	m_PreviousTime = m_CurrentTime;
	m_CurrentTime = std::chrono::steady_clock::now();
}
