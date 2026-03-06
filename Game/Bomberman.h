#pragma once
#include "Game.h"

class Bomberman final : public Game
{
public:
    Bomberman() = default;
    ~Bomberman() override;

    void Init() override;
    void Render() override;
private:

};