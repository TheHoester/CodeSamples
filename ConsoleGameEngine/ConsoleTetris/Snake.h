#pragma once
#include <queue>
#include <thread>
#include "Application.h"
#include "InputHandler.h"

using namespace std;

struct Vector2
{
	int x;
	int y;

	Vector2(void) : x(0), y(0) { }
	Vector2(int x, int y) : x(x), y(y) { }

	Vector2& operator=(Vector2 other)
	{
		std::swap(x, other.x);
		std::swap(y, other.y);
		return* this;
	}

	bool operator==(Vector2 other) { return (x == other.x && y == other.y); }
};

/**
 * Snake
 * The app that controls the logic and display of the snake game.
 */
class Snake : public Application
{
private:
	// Player
	int currentDirection;
	Vector2 currentPosition;

	// Gameplay
	const float movementDelay;
	float movementCounter;
	bool move;
	queue<Vector2> snake;

	// Game Logic Functions
	void InputHandler(void) override;
	void GameLogic(void) override;
	void Draw(void) override;

	// Misc Functions
	void GenerateAssets(void) override;
	void Move(int direction);
	void NewPellet(void);

public:
	Snake(RenderEngine* rend, Time* time, int screenWidth, int screenHeight, int fieldWidth, int fieldHeight);
	~Snake(void);

	bool Update(void) override;
	void Reset(void) override;
};