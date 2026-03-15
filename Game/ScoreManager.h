#pragma once
#include "Subject.h"
#include "Singleton.h"
#include <string>

namespace dae
{
	class ScoreManager final : public Singleton<ScoreManager>
	{
	public:
		void AddScore(unsigned int amount);
		void SetScore(unsigned int amount); // could be used as a reset or sumn else idk

		unsigned int GetScore() const { return m_Score; }
		std::string GetFormatedScore() const;

		Subject& GetSubject() { return m_Subject; }
	private:
		unsigned int m_Score{};
		Subject m_Subject{};
	};
}