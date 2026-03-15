#include "ScoreManager.h"
#include <sstream>
#include <iomanip>

void dae::ScoreManager::AddScore(unsigned int amount)
{
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
