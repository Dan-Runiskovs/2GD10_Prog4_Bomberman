#pragma once
#include <string>
#include <memory>
#include "ComponentBase.h"
#include <SDL3_ttf/SDL_ttf.h>

namespace dae
{
	class Font;
	class Texture2D;
	class RenderComponent;
	class TextComponent final : public  ComponentBase
	{
	public:
		static constexpr ComponentID ID{ ComponentID::Text };

		TextComponent(GameObject& owner, const std::string& text, std::shared_ptr<Font> font);

		void Update() override {}
		void UpdateText();

		const std::string GetText() const { return m_Text; } // why not... maybe will need
		const std::shared_ptr<Texture2D> GetTexture() const { return m_pTextTexture; }

		void SetText(const std::string& text);
		void SetFont(std::shared_ptr<Font> font);
		void SetColor(const SDL_Color& color);

		virtual ~TextComponent() = default;
		TextComponent(const TextComponent& other) = delete;
		TextComponent(TextComponent&& other) = delete;
		TextComponent& operator=(const TextComponent& other) = delete;
		TextComponent& operator=(TextComponent&& other) = delete;
	private:
		std::string m_Text{};
		SDL_Color m_Color{ 255, 255, 255, 255 };
		std::shared_ptr<Font> m_pFont{ nullptr };
		std::shared_ptr<Texture2D> m_pTextTexture{ nullptr };

		RenderComponent* m_pRenderComponent{ nullptr }; // Requires RenderComponent 
	};
}