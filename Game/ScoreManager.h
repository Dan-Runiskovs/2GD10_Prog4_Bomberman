#pragma once
#include "Subject.h"
#include "Singleton.h"
#include <string>
#include <cstdint>

namespace dae
{
	class ScoreManager final
	{
	public:
		ScoreManager(uint8_t playerIdx) noexcept;

		void AddScore(unsigned int amount);
		void SetScore(unsigned int amount); // could be used as a reset or sumn else idk

		unsigned int GetScore() const { return m_Score; }
		uint8_t GetAssignedPlayerIdx() const { return m_playerIdx; }
		std::string GetFormatedScore() const;

		Subject& GetSubject() { return m_Subject; }
	private:
		const uint8_t m_playerIdx;

		unsigned int m_Score{};
		Subject m_Subject{};
	};
}