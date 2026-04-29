#pragma once
#include "StatsAndAchievements.h"
#include "Singleton.h"
#include <vector>
#include <memory>

namespace dae
{
	enum class SteamAchievements
	{
		ACH_WIN_ONE_GAME = 0,
	};
	class ScoreManager;
	class AchievementManager final
	{
	public:
		AchievementManager();

		void CheckScoreForAchievement(ScoreManager& sm);

		~AchievementManager() = default;
		AchievementManager(const AchievementManager& other) = delete;
		AchievementManager(AchievementManager&& other) = delete;
		AchievementManager& operator=(const AchievementManager& other) = delete;
		AchievementManager& operator=(AchievementManager&& other) = delete;
	private:
		std::vector<Achievement_t> m_Achievements{};
		std::unique_ptr<CSteamAchievements> m_SteamAchievements{};

		const unsigned int m_ScoreToWin{ 500 };
	};
}