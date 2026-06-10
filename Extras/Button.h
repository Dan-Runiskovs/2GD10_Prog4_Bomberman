#pragma once

#include <glm/glm.hpp>
#include <string>

#include "Subject.h"
#include "Scene.h"

namespace dae
{
	class GameObject;
	class TextComponent;
	// TODO: TEMP implementation, will later be changed for textures
	class Button final
	{
	public:
		explicit Button(const glm::vec2& position, const std::string& text, dae::Scene& scene);
		
		void Click();
		void SetSelected(bool isSelected);
		void SetLock(bool newLock);

		Subject& GetSubject() { return m_Subject; }
	private:
		bool m_IsSelected{ false };
		bool m_IsLocked{ false };
		
		std::string m_TextBase;
		TextComponent* m_TextComponent;
		Subject m_Subject{};
	};
}