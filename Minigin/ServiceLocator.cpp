#include "ServiceLocator.h"

std::unique_ptr<dae::SoundSystem> dae::ServiceLocator::m_SSInstance{ std::make_unique<dae::NullSoundSystem>() };

void dae::ServiceLocator::RegisterSoundSystem(std::unique_ptr<SoundSystem>&& ss)
{
	m_SSInstance = (ss == nullptr) ? std::make_unique<NullSoundSystem>() : std::move(ss);	
}
