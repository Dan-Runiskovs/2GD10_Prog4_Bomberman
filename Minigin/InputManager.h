#pragma once
#include "Singleton.h"

#include <memory>
#include <vector>
#include <cstdint>

#include "Controller.h"
#include "Bindings.h"

namespace dae
{
    class InputManager final : public Singleton<InputManager>
    {
    public:
        bool ProcessInput();
        void QueueExit() { m_Exit = true; }

        Controller& AddController(uint8_t id);
        Controller& GetController(uint8_t id);
        uint8_t GetControllerAmount();

        void AddBinding(std::unique_ptr<Binding> binding);
        void ClearBindings();
        void ClearControllerBindings(uint8_t controllerID);
    private:
        std::vector<std::unique_ptr<Controller>> m_Controllers;
        std::vector<std::unique_ptr<Binding>> m_Bindings;

        bool m_Exit{ false };
    };
}
