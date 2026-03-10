#include "Bindings.h"
#include "Controller.h"

// --- Base ---
dae::Binding::Binding(std::unique_ptr<Command> command, CommandType type) noexcept
	: m_pCommand{ std::move(command)}
	, m_CommandType{ type }
{
}

// --- Keyboard ---
dae::KeyboardBinding::KeyboardBinding(SDL_Scancode key, std::unique_ptr<Command> command, CommandType type) noexcept
	: Binding(std::move(command), type)
	, m_Key{ key }
{
}

void dae::KeyboardBinding::TryExecute()
{
	const bool* keyboard{ SDL_GetKeyboardState(nullptr) };
	bool isPressed{ keyboard[m_Key] };
	bool isTriggered{ false };

	switch (m_CommandType)
	{
	case dae::CommandType::OnPress:
		isTriggered = isPressed && !m_PreviousState;
		break;
	case dae::CommandType::OnHold:
		isTriggered = isPressed;
		break;
	case dae::CommandType::OnRelease:
		isTriggered = !isPressed && m_PreviousState;
		break;
	}

	if (isTriggered) m_pCommand->Execute();
	m_PreviousState = isPressed;
}

// --- Controller ---
dae::ControllerBinding::ControllerBinding(Controller& controller, dae::ControllerButton button, std::unique_ptr<Command> command, CommandType type) noexcept
	: Binding(std::move(command), type)
	, m_Controller(controller)
	, m_Button(button)
{
}

void dae::ControllerBinding::TryExecute()
{
	bool isTriggered{ false };

	switch (m_CommandType)
	{
	case dae::CommandType::OnPress:
		isTriggered = m_Controller.IsButtonDownThisFrame(m_Button);
		break;
	case dae::CommandType::OnHold:
		isTriggered = m_Controller.IsButtonPressed(m_Button);
		break;
	case dae::CommandType::OnRelease:
		isTriggered = m_Controller.IsButtonUpThisFrame(m_Button);
		break;
	}

	if (isTriggered) m_pCommand->Execute();
}
