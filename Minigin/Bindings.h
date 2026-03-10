#pragma once
#include <memory>
#include <SDL3/SDL.h>
#include <cstdint>
#include "Commands.h"

namespace dae
{
	// --- Base ---
    enum class CommandType : uint8_t
    {
        OnPress = 0,
        OnHold = 1,
        OnRelease = 2
    };

    class Controller;
    enum class ControllerButton : unsigned int;
    class Binding
    {
    public:
        Binding(std::unique_ptr<Command> command, CommandType type) noexcept;
        virtual void TryExecute() = 0;

        virtual ~Binding() noexcept = default;
    protected:
        std::unique_ptr<Command> m_pCommand;
        CommandType m_CommandType;
    };

    // --- Derived ---
    class KeyboardBinding final : public Binding
    {
    public:
        KeyboardBinding(SDL_Scancode key, std::unique_ptr<Command> command, CommandType type) noexcept;
        void TryExecute() override;
    private:
        SDL_Scancode m_Key;
        bool m_PreviousState{ false };
    };

    class ControllerBinding final : public Binding
    {
    public:
        ControllerBinding(Controller& controller, ControllerButton button, std::unique_ptr<Command> command, CommandType type) noexcept;
        void TryExecute() override;
    private:
        Controller& m_Controller;
        ControllerButton m_Button;
    };
}