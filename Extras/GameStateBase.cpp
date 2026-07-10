#include "GameStateBase.h"
#include "InputManager.h"
#include "Bindings.h"
#include "CustomCommands.h"
#include "ServiceLocator.h"
#include "Utils.h"
#include "Game.h"
#include "Bomberman.h"

using dae::Utils::HexToSDLColor;
namespace HCol = dae::Utils::Colors;
using dae::Utils::Sounds;

dae::GameState::GameState(Game& game)
	: m_Game(game)
{}

#pragma region Helpers
void dae::GameState::RotateButtonSelection(bool isNext)
{
    // --- Early return if there is only one button ---
    if (m_SceneButtons.size() <= 1) return;

    // --- Deselect current one ---
    m_SceneButtons[m_SelectedButtonIndex].SetSelected(false);

    // --- Get Potential outcome ---
    int potentialIdx = (isNext)
        ? static_cast<int>(m_SelectedButtonIndex) + 1
        : static_cast<int>(m_SelectedButtonIndex) - 1;

    // --- Bounds check ---
    if (isNext)
    {
        if (potentialIdx >= static_cast<int>(m_SceneButtons.size())) potentialIdx = 0;
    }
    else
    {
        if (potentialIdx < 0) potentialIdx = static_cast<int>(m_SceneButtons.size() - 1);
    }

    // --- Assign and Select
    m_SelectedButtonIndex = static_cast<uint8_t>(potentialIdx);
    m_SceneButtons[m_SelectedButtonIndex].SetSelected(true);
}

void dae::GameState::CreateMenuBindings(bool isVertical)
{
    // --- Select Default ---
    m_SelectedButtonIndex = 0;
    m_SceneButtons[m_SelectedButtonIndex].SetSelected(true);

    // --- Bindings ---
    auto& controllerRef = dae::InputManager::GetInstance().AddController(static_cast<uint8_t>(0));
    auto& input = InputManager::GetInstance();
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, (isVertical) ? dae::ControllerButton::GAMEPAD_DPAD_DOWN : dae::ControllerButton::GAMEPAD_DPAD_RIGHT,
        std::make_unique<dae::ExecuteCallbackCommand>([this]() {
            RotateButtonSelection(true);
            ServiceLocator::GetSoundSystem().PlaySFX(uint8_t(Sounds::Skip), 10);
            }),
        dae::CommandType::OnPress));
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, (isVertical) ? dae::ControllerButton::GAMEPAD_DPAD_UP : dae::ControllerButton::GAMEPAD_DPAD_LEFT,
        std::make_unique<dae::ExecuteCallbackCommand>([this]() {
            RotateButtonSelection(false);
            ServiceLocator::GetSoundSystem().PlaySFX(uint8_t(Sounds::Skip), 10);
            }),
        dae::CommandType::OnPress));
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, dae::ControllerButton::GAMEPAD_A,
        std::make_unique<dae::ExecuteCallbackCommand>([this]() {
            m_SceneButtons[m_SelectedButtonIndex].Click();
            ServiceLocator::GetSoundSystem().PlaySFX(uint8_t(Sounds::Select), 10);
            }),
        dae::CommandType::OnRelease));
}

void dae::GameState::ChangeState(std::unique_ptr<GameState> newState)
{
    m_Game.GetGameStateStack().ChangeState(std::move(newState));
}
#pragma endregion