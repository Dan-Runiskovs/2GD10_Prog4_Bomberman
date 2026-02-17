#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "TextComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Font.h"
#include "Texture2D.h"

dae::TextComponent::TextComponent(std::shared_ptr<GameObject> pOwner, const std::string& text, std::shared_ptr<Font> font, const SDL_Color& color)
	: ComponentBase(ComponentID::Text, std::move(pOwner))
	, m_NeedsUpdate{ true }
	, m_Text{ text }
	, m_pFont{ std::move(font) }
	, m_Color{ color }
	, m_pTextTexture{ nullptr }
{
}

void dae::TextComponent::Update()
{
	if (!m_NeedsUpdate) return;

	const auto surface = TTF_RenderText_Blended(m_pFont->GetFont(), m_Text.c_str(), m_Text.length(), m_Color);
	if (surface == nullptr)
	{
		throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
	}
	auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surface);
	if (texture == nullptr)
	{
		throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
	}
	SDL_DestroySurface(surface);
	m_pTextTexture = std::make_shared<Texture2D>(texture);
	m_NeedsUpdate = false;
}

void dae::TextComponent::SetText(const std::string& text)
{
	m_Text = text;
	m_NeedsUpdate = true;
}

void dae::TextComponent::SetColor(const SDL_Color& color)
{
	m_Color = color;
	m_NeedsUpdate = true;
}
