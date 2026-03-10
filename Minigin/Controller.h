#pragma once
#include <memory>
#include <cstdint>

namespace dae
{
    enum class ControllerButton : unsigned int
    {
        GAMEPAD_DPAD_UP         = 0x0001,
        GAMEPAD_DPAD_DOWN       = 0x0002,
        GAMEPAD_DPAD_LEFT       = 0x0004,
        GAMEPAD_DPAD_RIGHT      = 0x0008,
        GAMEPAD_START           = 0x0010,
        GAMEPAD_BACK            = 0x0020,
        GAMEPAD_LEFT_THUMB      = 0x0040,
        GAMEPAD_RIGHT_THUMB     = 0x0080,
        GAMEPAD_LEFT_SHOULDER   = 0x0100,
        GAMEPAD_RIGHT_SHOULDER  = 0x0200,
        GAMEPAD_A               = 0x1000,
        GAMEPAD_B               = 0x2000,
        GAMEPAD_X               = 0x4000,
        GAMEPAD_Y               = 0x8000
    };

    class Controller final
    {
    public:
        Controller(uint8_t id) noexcept;
        ~Controller() noexcept;

        bool IsConnected();

        void ProcessInput();

        bool IsButtonDownThisFrame(ControllerButton button) const;
        bool IsButtonUpThisFrame(ControllerButton button) const;
        bool IsButtonPressed(ControllerButton button) const;

        void Vibrate(uint16_t force = UINT16_MAX);
        void StopVibration();

        Controller(const Controller& other) noexcept = delete;
        Controller(Controller&& other) noexcept = delete;
        Controller& operator=(const Controller& other) noexcept = delete;
        Controller& operator=(Controller&& other) noexcept = delete;

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };
}