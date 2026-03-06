#pragma once
#include "Game.h"

class CommandPimpl final : public Game
{
public:
    CommandPimpl() = default;
    ~CommandPimpl() override;

    void Init() override;
    void Render() override;
private:

};