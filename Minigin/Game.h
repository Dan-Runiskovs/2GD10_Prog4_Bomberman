#pragma once
#include "SceneManager.h"
#include "Scene.h"

class Game
{
public:
	Game();
	virtual ~Game() = default;
	virtual void Init();
	virtual void Update();
	virtual void Render();
};