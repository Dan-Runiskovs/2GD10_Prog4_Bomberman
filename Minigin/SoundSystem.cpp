#include "SoundSystem.h"
#include <iostream>
#include <SDL3_mixer/SDL_mixer.h>
#include <unordered_map>
#include "ServiceLocator.h"

#pragma region pImpl
class dae::SinaiSoundSystem::Impl
{
public:
	struct MixerDeleter
	{
		void operator()(MIX_Mixer* pMixer) const
		{
			if (pMixer) MIX_DestroyMixer(pMixer);
		}
	};

	struct AudioDeleter
	{
		void operator()(MIX_Audio* pAudio) const
		{
			if (pAudio) MIX_DestroyAudio(pAudio);
		}
	};

	using MixerPtr = std::unique_ptr<MIX_Mixer, MixerDeleter>;
	using AudioPtr = std::unique_ptr<MIX_Audio, AudioDeleter>;

	Impl()
	{
		if (!MIX_Init())
		{
			std::cerr << "Failed to load SDL3_Mixer! Loading NullSoundSystem...\n";
			ServiceLocator::RegisterSoundSystem(std::make_unique<dae::NullSoundSystem>());
			return;
		}
		
		m_pMixer = MixerPtr(
			MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL)
		);

		m_Worker = std::jthread([this]
			{
				SoundEvent event;

				while (m_EventQueue.WaitAndPop(event))
				{
					event();
				}
			});
	}

	~Impl()
	{
		m_EventQueue.Stop();
		m_Worker.join();
		m_pMixer.reset();
		MIX_Quit();
	}

	void PlaySFX(const uint8_t sfxId, const uint8_t volume)
	{
		if (!m_SoundLib.contains(sfxId)) 
		{
			std::cout << "ERROR: Unknown SFX: " << sfxId << "!\n";
			return;
		}

		// --- Here the magic happens ---
		m_EventQueue.Push([=] {
			// --- Find stuff ---
			auto* audio = GetAutioPtr(sfxId);
			if (!audio) return;
			auto* pMixer = m_pMixer.get();

			// --- Set correct volume --
			MIX_SetMixerGain(pMixer, RemapToSDLVolume(volume));

			// --- Play the tune ---
			MIX_PlayAudio(pMixer, audio);
			}
		);
	}

	uint8_t LoadSFX(const SoundData& sound)
	{
		// --- Time to experiment, ladies and gentlemen ---
		auto promise{ std::make_shared<std::promise<uint8_t>>() };
		auto future{ promise->get_future() };

		// --- Wait until the value gets set ---
		m_EventQueue.Push([id = sound.id, path = sound.fullPath, promise, this]
			{
				if (m_SoundLib.contains(id))
				{
					std::cout << "ERROR: Provided ID already in use\n";
					promise->set_value(id);
					return;
				}

				// --- Not in the Librarty yet : Add this one ---
				auto sfx = AudioPtr(
					MIX_LoadAudio(m_pMixer.get(), path.data(), true)
				);

				// --- Safety check ---
				if (sfx) std::cout << "Load success?!\n";

				//--- Save and Set the flag ---
				m_SoundLib.emplace(id, std::move(sfx));
				promise->set_value(id);
			});

		// --- And now we wait ---
		return future.get();
	}

	void StopAll()
	{
		m_EventQueue.Push([this] 
			{
			MIX_StopAllTracks(m_pMixer.get(), 0);
			}
		);
	}
private:
	// --- Sound event queue ---
	SoundEventQueue m_EventQueue;
	std::jthread m_Worker; // My little soundboy :)
	MixerPtr m_pMixer;

	// --- Music Library ---
	std::unordered_map<uint8_t, AudioPtr> m_SoundLib;

	// --- Helpers ---
	float RemapToSDLVolume(uint8_t volume)
	{
		// @DEV -> In my engine volume goes [0:10]
		// However, SDL [0.f : 1.f]
		// Hence remapper
		return float(volume) / 10.f;
	}
	MIX_Audio* GetAutioPtr(uint8_t id)
	{
		auto it = m_SoundLib.find(id);
		if (it == m_SoundLib.end())
			return nullptr;

		return it->second.get();
	}
};
#pragma endregion

#pragma region Real SS

void dae::SinaiSoundSystem::PlaySFX(const uint8_t sfxId, const uint8_t volume)
{
	m_pImpl->PlaySFX(sfxId, volume);
}

uint8_t dae::SinaiSoundSystem::LoadSFX(const SoundData& sound)
{
	return m_pImpl->LoadSFX(sound);
}

void dae::SinaiSoundSystem::StopAll()
{
	m_pImpl->StopAll();
}

dae::SinaiSoundSystem::SinaiSoundSystem() noexcept
{
	m_pImpl = std::make_unique<Impl>();
}

// --- @DEV Dragged it over here, otherwise compiler complained ---
dae::SinaiSoundSystem::~SinaiSoundSystem() noexcept = default;

#pragma endregion

#pragma region Logging SS
dae::LoggingSoundSystem::LoggingSoundSystem(std::unique_ptr<SoundSystem>&& ss)
	: m_RealSoundSystem(std::move(ss))
{
	std::cout << "Created a Logging Sound System\n";
}

void dae::LoggingSoundSystem::PlaySFX(const uint8_t sfxId, const uint8_t volume)
{
	m_RealSoundSystem->PlaySFX(sfxId, volume);
	std::cout	<< "Playing SFX " << sfxId << "\n"
				<< "At Volume " << volume << "\n";
}

uint8_t dae::LoggingSoundSystem::LoadSFX(const SoundData& sound)
{
	std::cout << "Loading Sound:\n"
				<< "ID: " << sound.id << "\n"
				<< "Path: " << sound.fullPath << "\n";
	return m_RealSoundSystem->LoadSFX(sound);
}

void dae::LoggingSoundSystem::StopAll()
{
	m_RealSoundSystem->StopAll();
	std::cout << "Stopping All\n";
}
#pragma endregion