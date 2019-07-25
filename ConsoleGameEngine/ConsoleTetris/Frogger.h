#pragma once
# include <vector>

#include "Application.h"
#include "Sprite.h"

using namespace Engine::Graphics;

class Frogger : public Application
{
private:
	const int cellSize = 8;
	const int laneLength = 64;

	// Assets
	Sprite* bus;
	Sprite* car;
	Sprite* logLeft;
	Sprite* logMiddle;
	Sprite* logRight;
	Sprite* path;
	Sprite* wall;
	Sprite* water;
	Sprite* frog;

	std::vector<std::pair<float, std::wstring>> lanes;
	bool* dangerBuffer;

	// Player
	GameObject* player;

	// Game Logic Functions
	void GameLogic(void);
	void Draw(void);
	void Reset(void);

	// Misc Functions
	void GenerateAssets(void) override;

public:
	Frogger(GameEngine* engine, int appID, int width = 128, int height = 80, int fontWidth = 8, int fontHeight = 8);
	~Frogger(void);

	int Update(void) override;
};