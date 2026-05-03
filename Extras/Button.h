#pragma once

#include <glm/glm.hpp>
#include <string>

#include "Subject.h"
#include "Scene.h"

namespace dae
{
	class GameObject;
	class RenderComponent;
	class TextComponent;
	// TODO: TEMP implementation, will later be changed for textures
	class Button final
	{
	public:
		explicit Button(const glm::vec2& position, const std::string& text, dae::Scene& scene);
		
		void Click();
		void SetSelected(bool isSelected);

		Subject& GetSubject() { return m_Subject; }
	private:
		bool m_IsSelected{ false };
		
		//RenderComponent* m_RenderComponent;
		TextComponent* m_TextComponent;
		Subject m_Subject{};
	};
}