#include "SoundSystem.h"
#include <iostream>
#include <SDL3_mixer/SDL_mixer.h>
#include <unordered_map>
#include <thread>
#include "ServiceLocator.h"

#pragma region pImpl
class dae::SinaiSoundSystem::Impl
{
public:
	struct AudioDeleter
	{
		void operator()(MIX_Audio* pAudio) const
		{
			if (pAudio) MIX_DestroyAudio(pAudio);
		}
	};

	using AudioPtr = std::unique_ptr<MIX_Audio, AudioDeleter>;

	Impl(const std::filesystem::path& dataPath) 
		: m_pMixer(nullptr)
		, m_DataPath(dataPath)
	{
		if (!MIX_Init())
		{
			std::cerr << "Failed to load SDL3_Mixer! Loading NullSoundSystem...\n";
			ServiceLocator::RegisterSoundSystem(std::make_unique<dae::NullSoundSystem>());
			return;
		}
		
		if (m_pMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL); !m_pMixer)
		{
			std::cerr << "Failed to create mixer\n";
		}

#ifndef __EMSCRIPTEN__
		m_Worker = std::jthread([this]
			{
				SoundEvent event;

				while (m_EventQueue.WaitAndPop(event))
				{
					event();
				}
			});
#endif
	}

	~Impl()
	{
#ifndef __EMSCRIPTEN__
		m_EventQueue.Stop();
#endif
		m_SoundLib.clear();

		// --- Uncomment these following lines in order to crash on destroy ---
		// if(m_pMixer) MIX_DestroyMixer(m_pMixer);
		//MIX_Quit();
	}

	void PlaySFX(const uint8_t sfxId, const uint8_t volume)
	{
#ifdef __EMSCRIPTEN__

		// --- Browser fallback: direct playback ---
		auto* audio = GetAutioPtr(sfxId);

		if (!audio)
		{
			std::cerr << "ERROR: Unknown SFX: "
				<< static_cast<int>(sfxId) << "!\n";
			return;
		}

		MIX_SetMixerGain(m_pMixer, RemapToSDLVolume(volume));
		MIX_PlayAudio(m_pMixer, audio);

#else

		// --- Desktop threaded playback ---
		m_EventQueue.Push([=, this]
			{
				auto* audio = GetAutioPtr(sfxId);

				if (!audio)
				{
					std::cerr << "ERROR: Unknown SFX: " << static_cast<int>(sfxId) << "!\n";
					return;
				}

				MIX_SetMixerGain(m_pMixer,RemapToSDLVolume(volume));
				MIX_PlayAudio(m_pMixer, audio);
			});

#endif
	}

	uint8_t LoadSFX(const SoundData& sound)
	{
		const auto fullPath{ m_DataPath / sound.path };

		if (m_SoundLib.contains(sound.id))
		{
			std::cerr << "ERROR: Provided ID already in use\n";
			return sound.id;
		}

		auto sfx = AudioPtr(
			MIX_LoadAudio(m_pMixer, fullPath.string().c_str(), true)
		);

		if (sfx) std::cout << "Load success!\n";
		else std::cerr << "Error: load failed\n";

		m_SoundLib.emplace(sound.id, std::move(sfx));

		return sound.id;
	}

	void StopAll()
	{
#ifdef __EMSCRIPTEN__
		MIX_StopAllTracks(m_pMixer, 0);
#else
		m_EventQueue.Push([this]
			{
				MIX_StopAllTracks(m_pMixer, 0);
			}
		);
#endif	
	}
private:
	// --- Sound event queue ---
	SoundEventQueue m_EventQueue;
#ifndef __EMSCRIPTEN__
	std::jthread m_Worker;
#endif
	MIX_Mixer* m_pMixer;

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

	const std::filesystem::path m_DataPath;
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

dae::SinaiSoundSystem::SinaiSoundSystem(const std::filesystem::path& dataPath) noexcept
{
	m_pImpl = std::make_unique<Impl>(dataPath);
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
	std::cout	<< "Playing SFX " << static_cast<int>(sfxId) << "\n"
				<< "At Volume " << static_cast<int>(volume) << "\n";
	m_RealSoundSystem->PlaySFX(sfxId, volume);
}

uint8_t dae::LoggingSoundSystem::LoadSFX(const SoundData& sound)
{
	std::cout	<< "Loading Sound:\n"
				<< "ID: " << static_cast<int>(sound.id) << "\n"
				<< "Path: " << sound.path << "\n";
	return m_RealSoundSystem->LoadSFX(sound);
}

void dae::LoggingSoundSystem::StopAll()
{
	m_RealSoundSystem->StopAll();
	std::cout << "Stopping All\n";
}
#pragma endregion

#pragma region SoundEventQueue

#pragma endregion

void dae::SoundEventQueue::Push(SoundEvent e)
{
	// --- Lock and push ---
	{
		std::lock_guard lock(m_Mutex);
		m_SoundEvents.push(std::move(e));
	}
	// --- Wake up the worker thread ---
	m_CV.notify_one();
}

bool dae::SoundEventQueue::WaitAndPop(SoundEvent& outEvent)
{
	std::unique_lock<std::mutex> lock(m_Mutex);

	// --- Sleep till work arrives ---
	m_CV.wait(lock, [this]
		{
			return !m_SoundEvents.empty() || !m_Running;
		});

	if (!m_Running && m_SoundEvents.empty()) return false;

	// --- Not empty and running ? Get the sound out ---
	outEvent = std::move(m_SoundEvents.front());
	m_SoundEvents.pop();
	return true;
}

void dae::SoundEventQueue::Stop()
{
	// --- Lock the thread anbd stop running ---
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Running = false;
	}
	m_CV.notify_all();
}
