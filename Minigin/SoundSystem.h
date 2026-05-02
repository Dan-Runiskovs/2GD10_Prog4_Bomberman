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
#include <filesystem>

namespace dae
{
	namespace fs = std::filesystem;

#pragma region Base Interface
	// --- The Sound ---
	struct SoundData
	{
		const uint8_t id;
		const std::string_view path;
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

		SinaiSoundSystem(const std::filesystem::path& dataPath) noexcept;
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
		void Push(SoundEvent e);

		// --- Blocking pop ---
		bool WaitAndPop(SoundEvent& outEvent);
		
		// --- Stop SoundEventQueue ---
		void Stop();
		
	private:
		std::queue<SoundEvent> m_SoundEvents{};
		std::mutex m_Mutex{};
		std::condition_variable m_CV{};
		bool m_Running{ true };
	};
#pragma endregion
}