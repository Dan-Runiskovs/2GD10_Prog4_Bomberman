#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "TextComponent.h"
#include "RenderComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Font.h"
#include "Texture2D.h"


dae::TextComponent::TextComponent(GameObject& owner, const std::string& text, std::shared_ptr<Font> font)
	: ComponentBase(owner)
	, m_Text{ text }
	, m_pFont{std::move(font)}
{
	m_pRenderComponent = owner.TryGetComponent<RenderComponent>();
	if (!m_pRenderComponent)
	{
		m_pRenderComponent = owner.AddComponent<RenderComponent>();
	}
}

void dae::TextComponent::UpdateText()
{
	const auto pSurface = TTF_RenderText_Blended(m_pFont->GetFont(), m_Text.c_str(), m_Text.length(), m_Color);
	if (pSurface == nullptr)
	{
		throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
	}
	auto pTexture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), pSurface);
	if (pTexture == nullptr)
	{
		throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
	}
	SDL_DestroySurface(pSurface);
	m_pTextTexture = std::make_shared<Texture2D>(pTexture);
	m_pRenderComponent->SetTexture(m_pTextTexture);
}

void dae::TextComponent::SetText(const std::string& text)
{
	m_Text = text;
	UpdateText();
}

void dae::TextComponent::SetFont(std::shared_ptr<Font> font)
{
	m_pFont = font;
	UpdateText();
}

void dae::TextComponent::SetColor(const SDL_Color& color)
{
	m_Color = color;
	UpdateText();
}
