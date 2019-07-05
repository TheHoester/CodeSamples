#pragma once
#include <stack>
#include <vector>

#include "Application.h"
#include "GameEngine.h"

using namespace Engine;

/*
 * AutoMaze
 * Implements an algorithm that will create a random maze of any size starting from the top left of the screen.
 */
class AutoMaze : public Application
{
private:
	const int pathWidth = 3;
	const int mazeWidth;
	const int mazeHeight;

	std::stack<Vector2> path;
	bool* visited;
	int visitedCount;
	Vector2 startPosition;

	// Game Logic Functions
	void GameLogic(void) override;
	void Draw(void) override;
	void Reset(void);

	// Misc Functions
	void GenerateAssets(void) override;

	bool GetVisited(const int& x, const int& y) const;
	Vector2 GetRandomNeighbour(const int& x, const int& y) const;
	Vector2 GetRandomNeighbour(const Vector2& pos) const;

	void SetVisited(const int& x, const int& y);
	void SetVisited(const Vector2& pos);

public:
	AutoMaze(CHAR_INFO* screenBuffer, InputHandler* input, Time* time, int appID, int width = 160, int height = 80, int fontWidth = 8, int fontHeight = 8, int mazeWidth = 40, int mazeHeight = 20);
	~AutoMaze(void);

	int Update(void) override;
};