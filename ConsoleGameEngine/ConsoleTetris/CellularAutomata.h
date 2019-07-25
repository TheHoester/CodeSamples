#pragma once
#include "Application.h"
#include "GameEngine.h"

/*
 * CellularAutomata
 * Implements the logic for Conway's Game of Life
 */
class CellularAutomata : public Application
{
private:
	bool* currentState;
	bool* newState;

	// Game Logic Functions
	void GameLogic(void) override;
	void Draw(void) override;
	void Reset(void);

	// Misc Functions
	void GenerateAssets(void) override;
	int GetNeighbors(const int& x, const int& y);

public:
	CellularAutomata(GameEngine* engine, int appID, int width = 320, int height = 160, int fontWidth = 4, int fontHeight = 4);
	~CellularAutomata(void);

	int Update(void);
};