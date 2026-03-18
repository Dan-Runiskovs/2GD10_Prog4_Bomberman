#include "StatsAndAchievements.h"
#include <iostream>

CSteamAchievements::CSteamAchievements(Achievement_t* Achievements, int NumAchievements)
    : m_iAppID(0)
    , m_bInitialized(false)
#if USE_STEAMWORKS
    , m_CallbackUserStatsStored(this, &CSteamAchievements::OnUserStatsStored)
    , m_CallbackAchievementStored(this, &CSteamAchievements::OnAchievementStored)
#endif
{
#if USE_STEAMWORKS
    m_iAppID = SteamUtils()->GetAppID();
#endif
    m_pAchievements = Achievements;
    m_iNumAchievements = NumAchievements;
    m_bInitialized = Initialize();
}

CSteamAchievements::~CSteamAchievements()
{
}

bool CSteamAchievements::Initialize()
{
#if USE_STEAMWORKS
	// Is Steam loaded? If not we can't get stats.
	if (NULL == SteamUserStats() || NULL == SteamUser())
	{
		return false;
	}
	// Is the user logged on?  If not we can't access stats.
	if (!SteamUser()->BLoggedOn())
	{
		return false;
	}
#endif
	return true;
}

bool CSteamAchievements::SetAchievement(const char* ID)
{
#if USE_STEAMWORKS
	// Have we received a call back from Steam yet?
	if (m_bInitialized)
	{
		SteamUserStats()->SetAchievement(ID);
		return SteamUserStats()->StoreStats();
	}
#endif
	// If not then we can't set achievements yet
	return false;
}

#if USE_STEAMWORKS
void CSteamAchievements::OnUserStatsStored(UserStatsStored_t* pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID == static_cast<int64>(pCallback->m_nGameID))
	{
		if (k_EResultOK == pCallback->m_eResult)
		{
			std::cout << "Stored stats for Steam\n";
		}
		else
		{
			std::cout << "StatsStored - failed, %d\n" << pCallback->m_eResult << "\n";
		}
	}
}

void CSteamAchievements::OnAchievementStored(UserAchievementStored_t* pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID == static_cast<int64>(pCallback->m_nGameID))
	{
		std::cout << "Stored Achievement for Steam\n";
	}
}
#endif