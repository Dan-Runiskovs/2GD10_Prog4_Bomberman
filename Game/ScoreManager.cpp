#include "ScoreManager.h"
#include <sstream>
#include <iomanip>

//TODO: Remove in production
#include <iostream>

dae::ScoreManager::ScoreManager(uint8_t playerIdx) noexcept
	:m_playerIdx{playerIdx}
{
}

void dae::ScoreManager::AddScore(unsigned int amount)
{
	std::cout << "Adding score to player " << m_playerIdx << "!\n";
	m_Score += amount;
	m_Subject.Notify(Event::OnScoreChanged);
}

void dae::ScoreManager::SetScore(unsigned int amount)
{
	m_Score = amount;
	m_Subject.Notify(Event::OnScoreChanged);
}

std::string dae::ScoreManager::GetFormatedScore() const
{
	std::ostringstream ss;
	ss << std::setw(7) << std::setfill('0') << m_Score;
	return ss.str();
}
