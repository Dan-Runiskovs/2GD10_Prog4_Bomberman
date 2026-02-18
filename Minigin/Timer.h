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

		void Update(); // to be replaced by Update Component?
		// Add FPS component?

	private:
		bool m_IsRunning{ false };
		const short m_DesiredFPS{ 60 };

		std::chrono::steady_clock::time_point m_PreviousTime;
		std::chrono::steady_clock::time_point m_CurrentTime;
		const std::chrono::duration<float> m_MsPerFrame{ 1000.f / m_DesiredFPS };
	};
}