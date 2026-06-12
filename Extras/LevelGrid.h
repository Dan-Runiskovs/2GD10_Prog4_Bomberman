#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <filesystem>
#include <SDL3/SDL_rect.h>
#include <vector>
#include <memory>
#include "Texture2D.h"


namespace dae
{
    class Scene;
    class LevelGrid final
    {
    public:
        struct PropAmount
        {
            int barrelN;
            int bombUpgradeN;
            int blastUpgradeN;
            int speedUpgradeN;
        };

        enum class CellType : uint8_t
        {
            Empty,  // allowed to become a barrel
            Free,   // forced to stay empty
            Wall,
            Barrel,
            Spawn
        };

        struct GridCell
        {
            explicit GridCell(uint8_t x, uint8_t y, int cellSizePx);
            uint8_t x{};
            uint8_t y{};

            glm::vec2 center{};
            int m_CellSizePx{};

            CellType type{ CellType::Empty };

            uint8_t spawnIdx{ 255 }; // valid if Spawn
            const SDL_Rect GetBounds() const;
        };

        void InitLevelGrid(std::filesystem::path path, const glm::vec2 gridTopLeft, int cellSize);

        void VisualiseProps(Scene& scene, PropAmount& pa);

        void VisualiseBaseGrid(Scene& scene);

        glm::vec2 GetSpawnpoint(int playerIdx) const;

        GridCell& At(uint8_t x, uint8_t y);
        const GridCell& At(uint8_t x, uint8_t y) const;

        uint8_t GetWidth() const { return m_Width; }
        uint8_t GetHeight() const { return m_Height; }

    private:
        std::vector<GridCell> m_Cells{};
        uint8_t m_Width{};
        uint8_t m_Height{};
    };
}