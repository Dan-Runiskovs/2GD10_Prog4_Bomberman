#include "LevelGrid.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <random>
#include <string>
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"

void dae::LevelGrid::InitLevelGrid(std::filesystem::path path, const glm::vec2 gridTopLeft, int cellSize)
{
    // --- 1. Load file ---
    std::ifstream file( path );

    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open level file");
    }

    std::vector<std::string> csvRows{};

    std::string line{};
    while (std::getline(file, line))
    {
        std::stringstream ss{ line };

        std::string row{};

        std::string token{};
        while (std::getline(ss, token, ','))
        {
            row.push_back(token[0]);
        }

        csvRows.push_back(std::move(row));
    }

    file.close();

    // --- 2. Create Gid ---
    
    m_Height = static_cast<uint8_t>(csvRows.size());
    m_Width = csvRows.empty()
        ? 0
        : static_cast<uint8_t>(csvRows[0].length());

    std::cout << "Created grid! Width: " << std::to_string(static_cast<int>(m_Width))
                                         << " | Height: "
                                         << std::to_string(static_cast<int>(m_Height)) << "\n";

    m_Cells.reserve(m_Width * m_Height);

    for (uint8_t y{}; y < m_Height; ++y)
    {
        for (uint8_t x{}; x < m_Width; ++x)
        {
            // --- Create cell ---
            GridCell cell{ x, y, cellSize};

            // --- Calculate center ---
            cell.center =
            {
                gridTopLeft.x + x * cellSize + cellSize * 0.5f,
                gridTopLeft.y + y * cellSize + cellSize * 0.5f
            };

            const std::string& token{ csvRows[y][x] };

            if (token == "w")
            {
                cell.type = CellType::Wall;
            }
            else if (token == "f")
            {
                cell.type = CellType::Free;
            }
            else if (token == "0" ||
                     token == "1" ||
                     token == "2" ||
                     token == "3")
            {
                cell.type = CellType::Spawn;
                cell.spawnIdx = static_cast<uint8_t>(token[0] - '0');
            }
            else
            {
                // Candidate barrel location
                cell.type = CellType::Empty;
            }

            m_Cells.push_back(cell);
        }
    }

    // DONT ASK 0_0
    m_Cells[0].type = CellType::Wall;
}

void dae::LevelGrid::VisualiseProps(Scene& scene, PropAmount& pa)
{
    
    std::vector<GridCell*> barrelCandidates;

    for (auto& cell : m_Cells)
    {
        if (cell.type == CellType::Empty)
        {
            barrelCandidates.push_back(&cell);
        }
    }

    static std::random_device rd;
    static std::default_random_engine eng(rd());

    std::shuffle(
        barrelCandidates.begin(),
        barrelCandidates.end(),
        eng);

    pa.barrelN = std::min(
        pa.barrelN,
        static_cast<int>(barrelCandidates.size()));

    // --- Create and Visualise ---
    const auto cellSize = barrelCandidates[0]->m_CellSizePx;
    for (int ballerIdx{}; ballerIdx < pa.barrelN; ++ballerIdx)
    {
        barrelCandidates[ballerIdx]->type = CellType::Barrel;
        const glm::vec2 dimensions{ static_cast<float>(cellSize),
                                    static_cast<float>(cellSize) };
        auto go{ std::make_unique<dae::GameObject>() };
        go->AddComponent<dae::RenderComponent>();
        go->GetComponent<dae::RenderComponent>().SetTexture("cell_brick.png");
        go->GetComponent<dae::RenderComponent>().SetCentered(true);
        go->GetComponent<dae::RenderComponent>().SetDimensions(dimensions.x, dimensions.y);
        go->GetComponent<dae::TransformComponent>().SetWorldPosition(barrelCandidates[ballerIdx]->center);
        go->AddComponent<dae::PhysicsComponent>(scene, dimensions, 0.f, false);
        scene.Add(std::move(go));
    }
}


void dae::LevelGrid::VisualiseBaseGrid(Scene& scene)
{
    std::cout << "Visualising Grid base now!\n";
    // --- Base Start ---
    for (const auto& cell : m_Cells)
    {
        const glm::vec2 dimensions{ static_cast<float>(cell.m_CellSizePx),
                                    static_cast<float>(cell.m_CellSizePx) };
        auto go{ std::make_unique<dae::GameObject>() };
        go->AddComponent<dae::RenderComponent>();
        // --- Set Texture ---
        switch (cell.type)
        {
        case CellType::Wall:
            go->GetComponent<dae::RenderComponent>().SetTexture("cell_wall.png");
            go->AddComponent<dae::PhysicsComponent>(scene, dimensions, 0.f, false);
            break;
        case CellType::Empty:
        case CellType::Free:
        default:
            go->GetComponent<dae::RenderComponent>().SetTexture("cell_empty.png");
            break;
        }
        go->GetComponent<dae::RenderComponent>().SetDimensions(
            dimensions.x, 
            dimensions.y);
        go->GetComponent<dae::TransformComponent>().SetWorldPosition(cell.center);
        go->GetComponent<dae::RenderComponent>().SetCentered(true);
        scene.Add(std::move(go));
    }
}

glm::vec2 dae::LevelGrid::GetSpawnpoint(int playerIdx) const
{
    for (const auto& cell : m_Cells)
    {
        if (cell.type == CellType::Spawn)
        {
            if (cell.spawnIdx == static_cast<uint8_t>(playerIdx))
            {
                return cell.center;
            }
        }
    }
    return { 0,0 };
}

dae::LevelGrid::GridCell& dae::LevelGrid::At(uint8_t x, uint8_t y)
{
	return m_Cells[y * m_Width + x];
}

const dae::LevelGrid::GridCell& dae::LevelGrid::At(uint8_t x, uint8_t y) const
{
	return m_Cells[y * m_Width + x];
}

dae::LevelGrid::GridCell::GridCell(uint8_t xPos, uint8_t yPos, int cellSizePx)
	: x(xPos)
	, y(yPos)
	, m_CellSizePx(cellSizePx)
{
}

const SDL_Rect dae::LevelGrid::GridCell::GetBounds() const
{
	return SDL_Rect(
		static_cast<int>(center.x - m_CellSizePx * 0.5f),
		static_cast<int>(center.y - m_CellSizePx * 0.5f),
		m_CellSizePx, m_CellSizePx
		);
}
