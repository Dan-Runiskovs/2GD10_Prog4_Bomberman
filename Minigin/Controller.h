#pragma once
#include <memory>
#include <cstdint>

namespace dae
{
    class Controller final
    {
    public:
        Controller(uint8_t id) noexcept;
        ~Controller() noexcept;

        bool IsConnected();

        void ProcessInput();

        bool IsButtonDownThisFrame(unsigned int button) const;
        bool IsButtonUpThisFrame(unsigned int button) const;
        bool IsButtonPressed(unsigned int button) const;

        void Vibrate(uint16_t force = UINT16_MAX);
        void StopVibration();

        //TODO: static GetButtons() method

        Controller(const Controller& other) noexcept = delete;
        Controller(Controller&& other) noexcept = delete;
        Controller& operator=(const Controller& other) noexcept = delete;
        Controller& operator=(Controller&& other) noexcept = delete;

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };
}