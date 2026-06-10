#include "LetterSelector.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "Utils.h"
#include <memory>
#include <iostream>

using dae::Utils::HexToSDLColor;
namespace HCol = dae::Utils::Colors;

dae::LetterSelector::LetterSelector(const glm::vec2& position, dae::Scene& scene) noexcept
{
    auto go{ std::make_unique<dae::GameObject>() };
    const float displacement{ 50.f };
    auto letterFont{ dae::ResourceManager::GetInstance().LoadFont("SubFont.ttf", 50) };
    auto arrowFont{ dae::ResourceManager::GetInstance().LoadFont("Arrows.ttf", 25) };
    // --- Arrow up 'X' ---
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(position.x, position.y - displacement);
    go->AddComponent<dae::RenderComponent>();
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    m_Arrows[0] = &go->AddComponent<dae::TextComponent>('X', arrowFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::BLACK));
    scene.Add(std::move(go));

    // --- Arrow down 'x' ---
    go = std::make_unique<dae::GameObject>();
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(position.x, position.y + displacement);
    go->AddComponent<dae::RenderComponent>();
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    m_Arrows[1] = &go->AddComponent<dae::TextComponent>('x', arrowFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::BLACK));
    scene.Add(std::move(go));

    // --- The letter itself ---
    go = std::make_unique<dae::GameObject>();
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(position);
    go->AddComponent<dae::RenderComponent>();
    go->GetComponent<dae::RenderComponent>().SetCentered(true);

    m_pLetterText = &go->AddComponent<dae::TextComponent>(m_Letter, letterFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::WHITE));

    scene.Add(std::move(go));
}

void dae::LetterSelector::SetSelected(bool isSelected)
{
    if (m_IsLocked) return;

    m_IsSelected = isSelected;

    if (m_IsSelected)
    {
        // --- Arrows ---
        m_Arrows[0]->SetColor(HexToSDLColor(HCol::YELLOW));
        m_Arrows[1]->SetColor(HexToSDLColor(HCol::YELLOW));

        // --- Letter --- 
        m_pLetterText->SetColor(HexToSDLColor(HCol::YELLOW));
    }
    else
    {
        // --- Hide Arrows ---
        m_Arrows[0]->SetColor(HexToSDLColor(HCol::BLACK));
        m_Arrows[1]->SetColor(HexToSDLColor(HCol::BLACK));

        // --- Letter --- 
        m_pLetterText->SetColor(HexToSDLColor(HCol::WHITE));
    }
}

void dae::LetterSelector::RotateLetter(bool isNext)
{
    if (m_IsLocked) return;

    if (isNext)
    {
        m_Letter = (m_Letter == 'Z')
            ? 'A'
            : static_cast<char>(m_Letter + 1);
    }
    else
    {
        m_Letter = (m_Letter == 'A')
            ? 'Z'
            : static_cast<char>(m_Letter - 1);
    }

    m_pLetterText->SetText(std::string(1, m_Letter));
}

char dae::LetterSelector::GetLetter()
{
    return m_Letter;
}

void dae::LetterSelector::SetLock(bool newLock)
{
    m_IsLocked = newLock;
    // Since always is locking but still
    m_pLetterText->SetColor(HexToSDLColor(HCol::GREY));
}
