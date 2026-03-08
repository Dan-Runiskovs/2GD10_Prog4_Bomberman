#pragma once
#include "Game.h"

class InputTest final : public Game
{
public:
    InputTest() = default;
    ~InputTest() override = default;

    void Init() override;
    void Update() override;
    void Render() override;

private:
};