#include "Achievements.h"
#include "ScoreManager.h"

#if USE_STEAMWORKS
#pragma warning (push)
#pragma warning (disable:4996)
#include "steam_api.h"
#pragma warning (pop)
#endif

dae::AchievementManager::AchievementManager()
	: m_Achievements{ _ACH_ID(static_cast<int>(SteamAchievements::ACH_WIN_ONE_GAME), "Winner") }
	, m_SteamAchievements{ std::make_unique<CSteamAchievements>(m_Achievements.data(), static_cast<int>(m_Achievements.size())) }
{
}

void dae::AchievementManager::CheckScoreForAchievement(ScoreManager& sm)
{
	if (sm.GetScore() >= m_ScoreToWin)
	{
		m_SteamAchievements->SetAchievement("ACH_WIN_ONE_GAME");
	}
}
