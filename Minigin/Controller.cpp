#include "Controller.h"
#include <algorithm>
#include <memory>
#include <iostream>

#if _WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <XInput.h>
#pragma comment(lib, "XInput.lib")

#else

#include <SDL3/SDL_gamepad.h>

#endif

class dae::Controller::Impl
{
public:
    Impl(int id)
        :m_ID{id}
    {
#if !_WIN32
        m_pGamepad = SDL_OpenGamepad(*SDL_GetGamepads(&m_ID));
        if (m_pGamepad)
        {
            std::cout << "Gamepad Found!\n";
        }
        else
        {
            std::cout << "Gamepad not found!\n";
            std::cout << SDL_GetError();
        }
#endif
    }

    ~Impl()
    {
#if !_WIN32
        if (m_pGamepad) SDL_CloseGamepad(m_pGamepad);
#endif
    }

    bool IsConnected()
    {
#if _WIN32
        XINPUT_STATE state{};
        return XInputGetState(m_ID, &state) == ERROR_SUCCESS;
#else
        if (!m_pGamepad) return false;
        return SDL_GamepadConnected(m_pGamepad);
#endif
    }

    void ProcessInput()
    {
#if _WIN32
        CopyMemory(&m_PreviousState, &m_CurrentState, sizeof(XINPUT_STATE));
        ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
        XInputGetState(m_ID, &m_CurrentState);

        auto buttonChanges{ m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons };
        m_buttonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
        m_buttonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
#else
        m_PreviousButtons = m_CurrentButtons;
        m_CurrentButtons = 0;

        if (!m_pGamepad)
        {
            //std::cout << "No Gamepad found!\n";
            m_pGamepad = SDL_OpenGamepad(*SDL_GetGamepads(&m_ID));
            return;
        }

        auto remapCheck = [&](SDL_GamepadButton button, unsigned int mask)
            {
                if (SDL_GetGamepadButton(m_pGamepad, button))
                    // bitwise or to "turn on" the buttons that are currently pressed
                    m_CurrentButtons |= mask;
            };

        remapCheck(SDL_GAMEPAD_BUTTON_DPAD_UP,           static_cast<unsigned int>(ControllerButton::GAMEPAD_DPAD_UP));
        remapCheck(SDL_GAMEPAD_BUTTON_DPAD_DOWN,         static_cast<unsigned int>(ControllerButton::GAMEPAD_DPAD_DOWN));
        remapCheck(SDL_GAMEPAD_BUTTON_DPAD_LEFT,         static_cast<unsigned int>(ControllerButton::GAMEPAD_DPAD_LEFT));
        remapCheck(SDL_GAMEPAD_BUTTON_DPAD_RIGHT,        static_cast<unsigned int>(ControllerButton::GAMEPAD_DPAD_RIGHT));
        remapCheck(SDL_GAMEPAD_BUTTON_START,             static_cast<unsigned int>(ControllerButton::GAMEPAD_START));
        remapCheck(SDL_GAMEPAD_BUTTON_BACK,              static_cast<unsigned int>(ControllerButton::GAMEPAD_BACK));
        remapCheck(SDL_GAMEPAD_BUTTON_LEFT_STICK,        static_cast<unsigned int>(ControllerButton::GAMEPAD_LEFT_THUMB));
        remapCheck(SDL_GAMEPAD_BUTTON_RIGHT_STICK,       static_cast<unsigned int>(ControllerButton::GAMEPAD_RIGHT_THUMB));
        remapCheck(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,     static_cast<unsigned int>(ControllerButton::GAMEPAD_LEFT_SHOULDER));
        remapCheck(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,    static_cast<unsigned int>(ControllerButton::GAMEPAD_RIGHT_SHOULDER));
        remapCheck(SDL_GAMEPAD_BUTTON_SOUTH,             static_cast<unsigned int>(ControllerButton::GAMEPAD_A));
        remapCheck(SDL_GAMEPAD_BUTTON_EAST,              static_cast<unsigned int>(ControllerButton::GAMEPAD_B));
        remapCheck(SDL_GAMEPAD_BUTTON_WEST,              static_cast<unsigned int>(ControllerButton::GAMEPAD_X));
        remapCheck(SDL_GAMEPAD_BUTTON_NORTH,             static_cast<unsigned int>(ControllerButton::GAMEPAD_Y));

        auto buttonChanges = m_CurrentButtons ^ m_PreviousButtons;
        m_buttonsPressedThisFrame = buttonChanges & m_CurrentButtons;
        m_buttonsReleasedThisFrame = buttonChanges & (~m_CurrentButtons);
#endif
    }

    bool IsButtonDownThisFrame(unsigned int button) const
    {
        return m_buttonsPressedThisFrame & button;
    }

    bool IsButtonUpThisFrame(unsigned int button) const
    {
        return m_buttonsReleasedThisFrame & button;
    }

    bool IsButtonPressed(unsigned int button) const
    {
#if _WIN32
        return m_CurrentState.Gamepad.wButtons & button;
#else
        return m_CurrentButtons & button;
#endif
    }

    void Vibrate(uint16_t force)
    {
#if _WIN32

        const uint16_t maxForce{ UINT16_MAX };
        const auto vibrForce = std::clamp(force, static_cast<uint16_t>(0), maxForce);

        XINPUT_VIBRATION vibration{};
        vibration.wLeftMotorSpeed = static_cast<WORD>(vibrForce);
        vibration.wRightMotorSpeed = static_cast<WORD>(vibrForce);

        XInputSetState(m_ID, &vibration);
#else

        if (m_pGamepad)
        {
            SDL_RumbleGamepad(
                m_pGamepad,
                force,
                force,
                100
            );
        }

#endif
    }

    void StopVibration()
    {
        Vibrate(0);
    }

private:

    int m_ID{};

#if _WIN32

    XINPUT_STATE m_CurrentState{};
    XINPUT_STATE m_PreviousState{};
#else

    SDL_Gamepad* m_pGamepad{ nullptr };

    unsigned int m_CurrentButtons{};
    unsigned int m_PreviousButtons{};

#endif

    unsigned int m_buttonsPressedThisFrame{};
    unsigned int m_buttonsReleasedThisFrame{};
};

dae::Controller::Controller(int id) noexcept
{
    m_pImpl = std::make_unique<Impl>(id);
}

// default here, otherwise compiler goes nuts
dae::Controller::~Controller() noexcept = default;


bool dae::Controller::IsConnected()
{
    return m_pImpl->IsConnected();
}

void dae::Controller::ProcessInput()
{
    m_pImpl->ProcessInput();
}

bool dae::Controller::IsButtonDownThisFrame(ControllerButton button) const
{
    return m_pImpl->IsButtonDownThisFrame(static_cast<unsigned int>(button));
}

bool dae::Controller::IsButtonUpThisFrame(ControllerButton button) const
{
    return m_pImpl->IsButtonUpThisFrame(static_cast<unsigned int>(button));
}

bool dae::Controller::IsButtonPressed(ControllerButton button) const
{
    return m_pImpl->IsButtonPressed(static_cast<unsigned int>(button));
}

void dae::Controller::Vibrate(uint16_t force)
{
    m_pImpl->Vibrate(force);
}

void dae::Controller::StopVibration()
{
    m_pImpl->StopVibration();
}