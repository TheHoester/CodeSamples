#pragma once
#include "GameEngine.h"
#include "InputHandler.h"
#include "Time.h"

using namespace Engine;

/**
 * Application
 * Used as the base for each individual game within the entire project.
 */
class Application
{
protected:
	GameEngine* engine;

	// Console Window
	const int screenWidth;
	const int screenHeight;
	const int fontWidth;
	const int fontHeight;

	// Gameplay
	const int appID;

	// Game Logic Functions
	virtual void GameLogic(void) = 0;
	virtual void Draw(void) = 0;

	// Misc Functions
	virtual void GenerateAssets(void) = 0;

	static unsigned char* GenerateFieldBox(int fieldWidth, int fieldHeight, int character);
	static unsigned char* GenerateFieldOpenTopBox(int fieldWidth, int fieldHeight, int character);

public:
	Application(GameEngine* engine, int appID, int width = 80, int height = 30, int fontWidth = 8, int fontHeight = 16);
	~Application(void);
	
	int ScreenWidth(void) const;
	int ScreenHeight(void) const;
	int FontWidth(void) const;
	int FontHeight(void) const;

	virtual int Update(void) = 0;
	virtual void Reset(void);
};