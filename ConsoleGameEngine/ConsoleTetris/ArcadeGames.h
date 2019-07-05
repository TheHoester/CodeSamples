#pragma once
#include "AutoMaze.h"
#include "BouncingBall.h"
#include "CellularAutomata.h"
#include "FirstPerson.h"
#include "GameEngine.h"
#include "MainMenu.h"
#include "Snake.h"
#include "Tetris.h"

using namespace Engine;

/*
 * ArcadeGames
 * The main class that controls the entire arcade game application.
 * Created in such a way that more games can be added with ease.
 */
class ArcadeGames : public GameEngine
{
private:
	const int numOfStates = 7;

	Application** gameStates;
	int state;

	// Overridden Functions
	bool CreateGame(void) override;
	bool RunGame(void) override;

	// Misc Functions
	void UpdateScreenBuffer(void);

public:
	ArcadeGames(std::wstring name = L"Arcade Games", int width = 80, int height = 30, int fontWidth = 8, int fontHeight = 16);
	~ArcadeGames(void);
};