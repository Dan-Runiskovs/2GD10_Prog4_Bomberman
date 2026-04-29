#pragma once
#include <cstdint>
#include <memory>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <string>
#include <string_view>

namespace dae
{
#pragma region Base Interface
	// --- The Sound ---
	struct SoundData
	{
		const uint8_t id;
		const std::string_view fullPath;
	};

	// --- Pure virtual base interface ---
	class SoundSystem
	{
	public:
		virtual ~SoundSystem() = default;
		
		virtual void PlaySFX(const uint8_t musicId, const uint8_t volume) = 0;

		virtual uint8_t LoadSFX(const SoundData& sound) = 0;

		virtual void StopAll() = 0;
	};
#pragma endregion

#pragma region Null SS
	// --- Default null SoundSystem ---
	class NullSoundSystem final : public SoundSystem
	{
	public:
		void PlaySFX(const uint8_t, const uint8_t) override {};

		uint8_t LoadSFX(const SoundData&) override { return 0; }

		void StopAll() override {};

		NullSoundSystem() noexcept = default;
		~NullSoundSystem() noexcept = default;
	};

#pragma endregion

#pragma region Real SS
	// --- Real Sound System ---
	// --- @Dev, Name chosen is a love-letter to bass music culture, ---
	// --- I could not help myself ---
	class SinaiSoundSystem final : public SoundSystem
	{
	public:
		// --- @Dev Volume = [0;10] ---
		void PlaySFX(const uint8_t musicId, const uint8_t volume) override;

		uint8_t LoadSFX(const SoundData& sound) override;

		void StopAll() override;

		SinaiSoundSystem() noexcept;
		~SinaiSoundSystem() noexcept;
	private:
		class Impl;
		std::unique_ptr<Impl> m_pImpl;
	};
#pragma endregion

#pragma region Logging SS
	// --- Logging Sound System ---
	class LoggingSoundSystem final : public SoundSystem
	{
	public:
		LoggingSoundSystem(std::unique_ptr<SoundSystem>&& ss);

		void PlaySFX(const uint8_t musicId, const uint8_t volume) override;

		uint8_t LoadSFX(const SoundData& sound) override;

		void StopAll() override;

		~LoggingSoundSystem() = default;
	private:
		std::unique_ptr<SoundSystem> m_RealSoundSystem;
	};
#pragma endregion

#pragma region Sound Queue
	using SoundEvent = std::function<void()>;

	class SoundEventQueue final
	{
	public:
		// --- Add sound to queue ---
		void Push(SoundEvent e)
		{
			// --- Lock and push ---
			{
				std::lock_guard lock(m_Mutex);
				m_SoundEvents.push(std::move(e));
			}
			// --- Wake up the worker thread ---
			m_CV.notify_one();
		}

		// Blocking pop
		bool WaitAndPop(SoundEvent& outEvent)
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

		void Stop()
		{
			// --- Lock the thread anbd stop running ---
			{
				std::lock_guard<std::mutex> lock(m_Mutex);
				m_Running = false;
			}
			m_CV.notify_all();
		}

	private:
		std::queue<SoundEvent> m_SoundEvents{};
		std::mutex m_Mutex{};
		std::condition_variable m_CV{};
		bool m_Running{ true };
	};
#pragma endregion
}