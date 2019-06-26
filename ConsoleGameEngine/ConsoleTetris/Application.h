#pragma once
#include "RenderEngine.h"

/**
 * Application
 * Used as the base for each individual game within the entire project.
 */
class Application
{
protected:
	// Assets
	unsigned char* field;
	int fieldWidth;
	int fieldHeight;

	// Renderer
	wchar_t* screen;
	RenderEngine* renderer;
	int screenWidth;
	int screenHeight;

	// Controls
	bool* keys;

	// Gameplay
	bool gameOver;
	int score;

	// Game Logic Functions
	virtual void InputHandler(void) = 0;
	virtual void GameLogic(void) = 0;
	virtual void Draw(void) = 0;

	// Misc Functions
	virtual void GenerateAssets(void) = 0;

	static unsigned char* GenerateFieldBox(int fieldWidth, int fieldHeight, int character);
	static unsigned char* GenerateFieldOpenTopBox(int fieldWidth, int fieldHeight, int character);

public:
	Application(RenderEngine* rend, int screenWidth, int screenHeight);
	~Application(void);

	int GetScore(void) const;

	virtual bool Update(void) = 0;
	virtual void Reset(void);
};