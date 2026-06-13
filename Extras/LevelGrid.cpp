#include "LevelGrid.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <random>
#include <string>
#include <algorithm>
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
            else if (token == "b")
            {
                cell.type = CellType::EnemyBalloom;
            }
            else if (token == "0" ||
                     token == "1" ||
                     token == "2" ||
                     token == "3")
            {
                cell.type = CellType::Spawn;
                cell.spawnIdx = static_cast<uint8_t>(token[0] - '0');
            }
            else if (token == "d")
            {
                cell.type = CellType::Door;
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
    GridCell* pToRemember{ nullptr };
    for (auto& cell : m_Cells)
    {
        if (cell.type == CellType::Door)
        {
            pToRemember = &cell;
        }
        if (cell.type == CellType::Empty || cell.type == CellType::Door)
        {
            barrelCandidates.push_back(&cell);
        }
    }

    static std::random_device rd;
    static std::default_random_engine eng(rd());

    // --- Shuffle the order ---
    std::shuffle(
        barrelCandidates.begin(),
        barrelCandidates.end(),
        eng);

    pa.barrelN = std::min(pa.barrelN,static_cast<int>(barrelCandidates.size()));
    
    // --- Hide upgrades ---
    using UpgradeType = dae::Upgrade::UpgradeType;
    std::vector<UpgradeType> hiddenUpgrades;
    for (int i{}; i < pa.bombUpgradeN; ++i)
    {
        hiddenUpgrades.push_back(UpgradeType::BombCount);
    }

    for (int i{}; i < pa.blastUpgradeN; ++i)
    {
        hiddenUpgrades.push_back(UpgradeType::BlastIncrease);
    }

    for (int i{}; i < pa.speedUpgradeN; ++i)
    {
        hiddenUpgrades.push_back(UpgradeType::Speed);
    }
    // --- Shuffle them as well ---
      // --- Can't hide more upgrades than barrels ---
    if (hiddenUpgrades.size() > static_cast<size_t>(pa.barrelN))
    {
        hiddenUpgrades.resize(pa.barrelN);
    }

    std::shuffle(hiddenUpgrades.begin(), hiddenUpgrades.end(), eng);

    // --- Guarantee a barrel on the door ---
        const auto cellSize = barrelCandidates[0]->m_CellSizePx;
        const glm::vec2 dimensions{ static_cast<float>(cellSize),
                                    static_cast<float>(cellSize) };
    if(pToRemember)
    {
        m_Upgrades.emplace_back(
            std::make_unique<dae::Upgrade>(
                scene, *pToRemember, Upgrade::UpgradeType::Exit));

        pToRemember->type = CellType::Barrel;
        // --- Create Barrel ---
        auto go{ std::make_unique<dae::GameObject>() };
        go->AddComponent<dae::RenderComponent>();
        go->GetComponent<dae::RenderComponent>().SetTexture("cell_brick.png");
        go->GetComponent<dae::RenderComponent>().SetCentered(true);
        go->GetComponent<dae::RenderComponent>().SetDimensions(dimensions.x, dimensions.y);
        go->GetComponent<dae::TransformComponent>().SetWorldPosition(pToRemember->center);
        go->AddComponent<dae::PhysicsComponent>(scene, dimensions, 0.f, false);
        std::cout << "Created barrel on door at: [" << std::to_string(static_cast<int>(pToRemember->x))
            << "][" << std::to_string(static_cast<int>(pToRemember->y)) << "]!\n";
        auto* barrelPtr = go.get();

        pToRemember->occupants.push_back({
            barrelPtr, GridCell::OccupantType::Barrel
            });

        scene.Add(std::move(go));
    }
    

    // --- Create and Visualise ---
    if (barrelCandidates.empty()) return;

    for (int barrelIdx{}; barrelIdx < pa.barrelN; ++barrelIdx)
    {
        auto* pCell{ barrelCandidates[barrelIdx] };

        if (pCell->type == CellType::Barrel) continue;
        pCell->type = CellType::Barrel;

        // --- Hide upgrade in this barrel? ---
        if (barrelIdx < static_cast<int>(hiddenUpgrades.size()))
        {
            // --- Create Upgrade ---
            const auto upgradeType{ hiddenUpgrades[barrelIdx] };
            m_Upgrades.emplace_back(
                std::make_unique<dae::Upgrade>(
                    scene, *pCell, upgradeType));

        }

        // --- Create Barrel ---
        auto go{ std::make_unique<dae::GameObject>() };
        go->AddComponent<dae::RenderComponent>();
        go->GetComponent<dae::RenderComponent>().SetTexture("cell_brick.png");
        go->GetComponent<dae::RenderComponent>().SetCentered(true);
        go->GetComponent<dae::RenderComponent>().SetDimensions(dimensions.x, dimensions.y);
        go->GetComponent<dae::TransformComponent>().SetWorldPosition(pCell->center);
        go->AddComponent<dae::PhysicsComponent>(scene, dimensions, 0.f, false);

        auto* barrelPtr = go.get();

        pCell->occupants.push_back({
            barrelPtr, GridCell::OccupantType::Barrel
            });

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
    for (const auto& cell : m_Cells)
    {
        if (cell.type == CellType::EnemyBalloom)
        {
            const glm::vec2 dimensions{ static_cast<float>(cell.m_CellSizePx),
                                    static_cast<float>(cell.m_CellSizePx) };
            m_Enemies.push_back(std::make_unique<dae::Balloom>(
                scene, *this, cell.center, dimensions.x));
            auto& enemy{ m_Enemies.back() };
            enemy.get()->GetSubject().AddObserver(
                [this, score = enemy->GetScoreValue()](Event e)
                {
                    if (e == Event::OnDeath)
                    {
                        m_Score += static_cast<uint16_t>(score);
                        m_Subject.Notify(Event::OnScoreChanged);
                    }
                });
        }
    }
}

void dae::LevelGrid::ProcessGrid(std::vector<std::unique_ptr<dae::Player>>& players)
{
    // --- 1. Process Upgrades ---
    for (auto& upgrade : m_Upgrades)
    {
        upgrade.get()->Update(players);
    }
    std::erase_if(
        m_Upgrades,
        [](const auto& upgrade)
        {
            return upgrade->IsCollected();
        });

    // --- 2. Process enemies ---
    for (auto& enemy : m_Enemies)
    {
        // TEMP SOLUTION FOR ONE ENEMY TYPE ONLY
        static_cast<Balloom*>(enemy.get())->Update(players);
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

dae::GridCell& dae::LevelGrid::At(uint8_t x, uint8_t y)
{
	return m_Cells[y * m_Width + x];
}

const dae::GridCell& dae::LevelGrid::At(uint8_t x, uint8_t y) const
{
	return m_Cells[y * m_Width + x];
}

const dae::GridCell& dae::LevelGrid::WorldPosToGridCell(const glm::vec2& pos) const
{
    const auto& firstCell{ At(0, 0) };
    const auto& cellSize{ At(0, 0).m_CellSizePx };

    const glm::vec2 gridTopLeft
    {
        firstCell.center.x - cellSize * 0.5f,
        firstCell.center.y - cellSize * 0.5f
    };
    int x
    {
        static_cast<int>(
            (pos.x - gridTopLeft.x) / cellSize)
    };

    int y
    {
        static_cast<int>(
            (pos.y - gridTopLeft.y) / cellSize)
    };

    x = std::clamp(x, 0, static_cast<int>(m_Width) - 1);
    y = std::clamp(y, 0, static_cast<int>(m_Height) - 1);

    return At(
        static_cast<uint8_t>(x),
        static_cast<uint8_t>(y));
}

dae::GridCell& dae::LevelGrid::WorldPosToGridCell(const glm::vec2& pos)
{
    const auto& firstCell{ At(0, 0) };
    const auto& cellSize{ At(0, 0).m_CellSizePx };

    const glm::vec2 gridTopLeft
    {
        firstCell.center.x - cellSize * 0.5f,
        firstCell.center.y - cellSize * 0.5f
    };
    int x
    {
        static_cast<int>(
            (pos.x - gridTopLeft.x) / cellSize)
    };

    int y
    {
        static_cast<int>(
            (pos.y - gridTopLeft.y) / cellSize)
    };

    x = std::clamp(x, 0, static_cast<int>(m_Width) - 1);
    y = std::clamp(y, 0, static_cast<int>(m_Height) - 1);

    return At(
        static_cast<uint8_t>(x),
        static_cast<uint8_t>(y));
}

glm::vec2 dae::LevelGrid::GetWorldDimensions() const
{
    const auto cellSize{ static_cast<float>(At(0, 0).m_CellSizePx) };
    return glm::vec2(
        m_Width * cellSize, 
        m_Height * cellSize);
}

dae::GridCell::GridCell(uint8_t xPos, uint8_t yPos, int cellSizePx)
	: x(xPos)
	, y(yPos)
	, m_CellSizePx(cellSizePx)
{
}

const SDL_Rect dae::GridCell::GetBounds() const
{
	return SDL_Rect(
		static_cast<int>(center.x - m_CellSizePx * 0.5f),
		static_cast<int>(center.y - m_CellSizePx * 0.5f),
		m_CellSizePx, m_CellSizePx
		);
}

