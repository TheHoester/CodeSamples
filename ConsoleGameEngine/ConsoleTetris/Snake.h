#pragma once
#include <queue>
#include <string>

#include "Application.h"
#include "GameEngine.h"
#include "InputHandler.h"

/**
 * Snake
 * The app that controls the logic and display of the snake game.
 */
class Snake : public Application
{
private:
	// Engine
	const int fieldWidth = 18;
	const int fieldHeight = 18;
	unsigned char* field;

	// Player
	int currentDirection;
	Vector2 currentPosition;

	// Gameplay
	const float movementDelay = 0.3f;
	float movementCounter;
	bool move;
	std::queue<Vector2> snake;
	int score;
	bool gameOver;

	// Game Logic Functions
	void GameLogic(void);
	void Draw(void);
	void Reset(void);

	// Misc Functions
	void GenerateAssets(void) override;
	void Move(const int& direction);
	void NewPellet(void);

public:
	Snake(GameEngine* engine, int appID, int width = 80, int height = 30, int fontWidth = 8, int fontHeight = 16);
	~Snake(void);

	int Update(void) override;
};