#include "MatchSession.h"
#include <iostream>

void dae::MatchSession::SetMode(GameMode mode)
{
#ifdef _DEBUG
	switch (mode)
	{
	case GameMode::Solo:
		std::cout << "Starting Solo Game...\n";
		break;
	case GameMode::Pvp:
		std::cout << "Starting PvP Game...\n";
		break;
	case GameMode::Coop:
		std::cout << "Starting Coop Game...\n";
		break;
	}
#endif // _DEBUG

	m_Mode = mode;
}

dae::MatchSession::GameMode dae::MatchSession::GetMode() const
{
	return m_Mode;
}

void dae::MatchSession::SetResult(const MatchResult& result)
{
#ifdef _DEBUG
	std::cout << "Game Over, new result set\n";
#endif // _DEBUG

	m_Result = result;
}

const dae::MatchSession::MatchResult& dae::MatchSession::GetResult() const
{
	return m_Result;
}
