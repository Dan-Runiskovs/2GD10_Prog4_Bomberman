#include "Button.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "TextComponent.h"
#include "RenderComponent.h"

#include <memory>

dae::Button::Button(const glm::vec2& position, const std::string& text, dae::Scene& scene)
{
    auto go{ std::make_unique<dae::GameObject>() };
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(position);
    go->AddComponent<dae::RenderComponent>();
    //m_RenderComponent = &go->GetComponent<dae::RenderComponent>();

    auto font{ dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20) };
    m_TextComponent = &go->AddComponent<dae::TextComponent>(text, font);
    go->GetComponent<dae::TextComponent>().SetColor({ 255, 0, 0, 255 }); // red

    scene.Add(std::move(go));
}

void dae::Button::Click()
{
    // --- No clicking if not selected ---
    if (!m_IsSelected) return;

    // TODO: Play click sound

    // --- TODO: Temp -> Change the color to white
    m_TextComponent->SetColor({ 255, 255, 255, 255 });

    // --- Notify observers ---
    m_Subject.Notify(Event::OnClick);
}

void dae::Button::SetSelected(bool isSelected)
{
    m_IsSelected = isSelected;

    // --- Update color ---
    m_TextComponent->SetColor((isSelected) 
        ? SDL_Color{ 255, 255, 0, 255 } 
        : SDL_Color{ 255, 0, 0, 255 });

}
