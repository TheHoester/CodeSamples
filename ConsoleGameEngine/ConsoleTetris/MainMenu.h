#pragma once
#include "Application.h"
#include "InputHandler.h"

using namespace std;

/**
 * MainMenu
 * The main menu of the project to given access to multiple different games.
 */
class MainMenu : public Application
{
private:
	// Assets
	const int numMenuOptions = 6;
	wstring* menuOptions;
	int menuWidth;
	int menuHeight;

	// Gameplay
	int currentOption;

	// Controls
	const int inputDelay = 8;
	int inputDelayCounter;
	bool calculateInput;

	// Game Logic Functions
	void GameLogic(void) override;
	void Draw(void) override;

	// Misc Functions
	void GenerateAssets(void) override;

public:
	MainMenu(CHAR_INFO* screenBuffer, InputHandler* input, Time* time, int appID, int width = 80, int height = 30, int fontWidth = 8, int fontHeight = 16);
	~MainMenu(void);

	int Update(void) override;
};