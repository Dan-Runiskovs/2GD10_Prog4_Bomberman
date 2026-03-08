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

        void AddController(uint8_t id);

        void AddBinding(std::unique_ptr<Binding> binding);

    private:

        std::vector<std::unique_ptr<Controller>> m_Controllers;
        std::vector<std::unique_ptr<Binding>> m_Bindings;
    };
}
