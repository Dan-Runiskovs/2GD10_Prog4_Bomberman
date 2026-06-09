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
            int score{};
            uint8_t aliveMask{};
            uint32_t winnerColor{};
        };

        void SetMode(GameMode mode);
        GameMode GetMode() const;

        void SetResult(const MatchResult& result);
        const MatchResult& GetResult() const;

    private:
        GameMode m_Mode{ GameMode::Solo };
        MatchResult m_Result{};
    };
}