#include "Button.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "TextComponent.h"
#include "RenderComponent.h"
#include "Utils.h"

#include <memory>

using dae::Utils::HexToSDLColor;
namespace HCol = dae::Utils::Colors;

dae::Button::Button(const glm::vec2& position, const std::string& text, dae::Scene& scene)
    :m_TextBase{text}
{
    auto go{ std::make_unique<dae::GameObject>() };
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(position);
    go->AddComponent<dae::RenderComponent>();
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    //m_RenderComponent = &go->GetComponent<dae::RenderComponent>();

    auto font{ dae::ResourceManager::GetInstance().LoadFont("SubFont.ttf", 50) };
    m_TextComponent = &go->AddComponent<dae::TextComponent>(text, font);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::WHITE)); // red

    scene.Add(std::move(go));
}

void dae::Button::Click()
{
    // --- No clicking if not selected ---
    if (!m_IsSelected) return;

    // --- Change the color to Yellow ---
    m_TextComponent->SetColor(HexToSDLColor(HCol::YELLOW));

    // --- Notify observers ---
    m_Subject.Notify(Event::OnClick);
}

void dae::Button::SetSelected(bool isSelected)
{
    m_IsSelected = isSelected;

    // --- Update ---
    if (isSelected)
    {
        const std::string newText{ "> " + m_TextBase + " <" };

        m_TextComponent->SetText(newText);
        m_TextComponent->SetColor(HexToSDLColor(HCol::RED));
    }
    else
    {
        m_TextComponent->SetText(m_TextBase);
        m_TextComponent->SetColor(HexToSDLColor(HCol::WHITE));
    }
}
