#include "ScoreManager.h"

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
	char buffer[256];
	sprintf_s(buffer, "%07d", m_Score);
	std::string str(buffer);
	return str;
}
