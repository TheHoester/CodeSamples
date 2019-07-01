#pragma once
#include <thread>
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
	wstring menuOptions[4];
	int menuWidth;
	int menuHeight;

	// Controls
	int inputDelay;
	int inputDelayCounter;
	bool calculateInput;

	// Gameplay
	int numMenuOptions;

	// Game Logic Functions
	void InputHandler(void) override;
	void GameLogic(void) override;
	void Draw(void) override;

	// Misc Functions
	void GenerateAssets(void) override;

public:
	MainMenu(RenderEngine* rend, Time* time, int screenWidth, int screenHeight);
	~MainMenu(void);

	bool Update(void) override;
};