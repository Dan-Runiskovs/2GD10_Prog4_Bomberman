#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <XInput.h>

#pragma comment(lib, "XInput.lib")

#include "Controller.h"
#include <algorithm>
#include <memory>

class dae::Controller::Impl
{
public:
    Impl(uint8_t id)
        : m_ID(id)
    {
    }

    bool IsConnected()
    {
        XINPUT_STATE state{};
        return XInputGetState(m_ID, &state) == ERROR_SUCCESS;
    }

    void ProcessInput()
    {
        CopyMemory(&m_PreviousState, &m_CurrentState, sizeof(XINPUT_STATE));
        ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
        XInputGetState(m_ID, &m_CurrentState);

        auto buttonChanges{ m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons };
        m_buttonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
        m_buttonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
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
        return m_CurrentState.Gamepad.wButtons & button;
    }

    void Vibrate(uint16_t force)
    {
        const uint16_t maxForce{ UINT16_MAX };
        const auto vibrForce = std::clamp(force, static_cast<uint16_t>(0), maxForce);

        XINPUT_VIBRATION vibration{};
        vibration.wLeftMotorSpeed = static_cast<WORD>(vibrForce);
        vibration.wRightMotorSpeed = static_cast<WORD>(vibrForce);

        XInputSetState(m_ID, &vibration);
    }

    void StopVibration()
    {
        Vibrate(0);
    }

private:

    uint8_t m_ID{};

    XINPUT_STATE m_CurrentState{};
    XINPUT_STATE m_PreviousState{};

    unsigned int m_buttonsPressedThisFrame{};
    unsigned int m_buttonsReleasedThisFrame{};
};

dae::Controller::Controller(uint8_t id) noexcept
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

bool dae::Controller::IsButtonDownThisFrame(unsigned int button) const
{
    return m_pImpl->IsButtonDownThisFrame(button);
}

bool dae::Controller::IsButtonUpThisFrame(unsigned int button) const
{
    return m_pImpl->IsButtonUpThisFrame(button);
}

bool dae::Controller::IsButtonPressed(unsigned int button) const
{
    return m_pImpl->IsButtonPressed(button);
}

void dae::Controller::Vibrate(uint16_t force)
{
    m_pImpl->Vibrate(force);
}

void dae::Controller::StopVibration()
{
    m_pImpl->StopVibration();
}