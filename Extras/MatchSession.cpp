#include "MatchSession.h"
#include <iostream>
#include <cassert>
#include <bit>
#include <bitset>
#include <string>

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

void dae::MatchSession::SetAmoundOfPlayers(uint8_t nPlayers)
{
	assert(nPlayers <= 4);
	m_Result.playerAmount = nPlayers;
	std::bitset<8> x(m_Result.aliveMask);
	std::cout << "Amount of players: " << x << '\n';
}

void dae::MatchSession::FillAliveMask(int playerAmount)
{
	// --- Fills alive mask 0b0000xxxx based on player amount
	assert(playerAmount > 0);
	assert(playerAmount <= 4);

	m_Result.aliveMask = static_cast<uint8_t>((1u << playerAmount) - 1);

	std::bitset<8> x(m_Result.aliveMask);
	std::cout << "Amount of players(AMF): " << x << '\n';
}

void dae::MatchSession::OnPlayerDead(int playerIndex)
{
	// unset the bit located at playerIndex'th bit
	std::cout << "Dead player Index: " << std::to_string(playerIndex) << "\n";
	std::bitset<8> x(m_Result.aliveMask);
	std::cout << "Before: " << x << '\n';

	m_Result.aliveMask = m_Result.aliveMask & ~(1 << playerIndex);

	std::bitset<8> y(m_Result.aliveMask);
	std::cout << "After: " << y << '\n';
}

void dae::MatchSession::SetLastPlayerAliveAsWinner()
{
	// counts amounts of 0 strating from right == that alive player idx
	std::bitset<8> x(m_Result.aliveMask);
	std::cout << "Alive mask for the win: " << x << '\n';
	uint8_t winIdx{ static_cast<uint8_t>(std::countr_zero(m_Result.aliveMask)) };
	std::cout << "Winner Index: " << std::to_string(static_cast<int>(winIdx)) << "!\n";
	m_Result.winnerIdx = winIdx;
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
	uint8_t nPlayers = m_Result.playerAmount;
	m_Result = result; // player amount gets overriden
	m_Result.playerAmount = nPlayers;
}

const dae::MatchSession::MatchResult& dae::MatchSession::GetResult() const
{
	return m_Result;
}

dae::MatchSession::MatchResult& dae::MatchSession::GetResult()
{
	return m_Result;
}
