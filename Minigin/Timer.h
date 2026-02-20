#pragma once
#include "Singleton.h"
#include <chrono>

namespace dae
{
	class Timer final : public Singleton<Timer>
	{
	public:
		void Start();
		void Stop();

		float GetElapsed() const;
		float GetMsPerFrame() const { return m_MsPerFrame.count(); }
		std::chrono::steady_clock::time_point GetTimeNow() const { return m_CurrentTime; }

		void Update();

	private:
		bool m_IsRunning{ false };
		const short m_DesiredFPS{ 60 };

		std::chrono::steady_clock::time_point m_PreviousTime;
		std::chrono::steady_clock::time_point m_CurrentTime;
		const std::chrono::duration<float> m_MsPerFrame{ 1.f / m_DesiredFPS };
	};
}