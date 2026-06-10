#pragma once
#include <glm/glm.hpp>
#include "Scene.h"
#include <array>

namespace dae
{
	class GameObject;
	class TextComponent;
	class LetterSelector final
	{
	public:
		explicit LetterSelector(const glm::vec2& position, dae::Scene& scene) noexcept;

		void SetSelected(bool isSelected);
		void RotateLetter(bool isNext);
		char GetLetter();
		void SetLock(bool newLock);

	private:
		char m_Letter{ 'A' };
		bool m_IsSelected{ false };
		bool m_IsLocked{ false };
		TextComponent* m_pLetterText;
		std::array<TextComponent*, 2> m_Arrows{};
	};
}