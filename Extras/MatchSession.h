#pragma once
#include <cstdint>

namespace dae
{
    class MatchSession
    {
    public:
        enum class GameMode : uint8_t
        {
            Solo = 1,
            Pvp = 2,
            Coop = 3,
        };

        struct MatchResult
        {
            bool isWin{};
            uint8_t aliveMask{};
            uint8_t winnerIdx{};
            uint8_t playerAmount{};
            uint16_t score{};
        };

        void SetMode(GameMode mode);
        void SetAmoundOfPlayers(uint8_t nPlayers);
        void FillAliveMask(int playerAmount);
        void OnPlayerDead(int playerIndex);
        void SetLastPlayerAliveAsWinner();
        GameMode GetMode() const;

        void SetResult(const MatchResult& result);
        const MatchResult& GetResult() const;
        MatchResult& GetResult();

    private:
        GameMode m_Mode{ GameMode::Solo };
        MatchResult m_Result{};
    };
}