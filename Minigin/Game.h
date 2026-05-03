#pragma once

namespace dae
{
    class Game
    {
    public:
        virtual ~Game() = default;

        virtual void Init() = 0;
        virtual void HandleInput() = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;
    };
}