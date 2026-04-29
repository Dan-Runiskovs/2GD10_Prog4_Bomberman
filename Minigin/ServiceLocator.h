#pragma once
#include <memory>
#include "SoundSystem.h"

namespace dae
{
	class ServiceLocator final
	{
	public:
		static SoundSystem& GetSoundSystem() { return *m_SSInstance; }
		static void RegisterSoundSystem(std::unique_ptr<SoundSystem>&& ss);
	private:
		static std::unique_ptr<SoundSystem> m_SSInstance;
	};
}