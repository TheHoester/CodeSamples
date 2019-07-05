#pragma once
#include "Application.h"
#include "GameEngine.h"
#include "InputHandler.h"

/*
 * Bouncing Ball
 * The app that controls the logic and display of the bouncing ball app.
 */
class BouncingBall : public Application
{
private:
	// Assets
	int radius;

	// Gameplay
	FVector2 position;
	FVector2 direction;
	float speed;

	// Game Logic Functions
	void GameLogic(void);
	void Draw(void);
	void Reset(void);

	// Misc Functions
	void GenerateAssets(void) override;

public:
	BouncingBall(CHAR_INFO* screenBuffer, InputHandler* input, Time* time, int appID, int width = 120, int height = 60, int fontWidth = 10, int fontHeight = 10, int radius = 2);
	~BouncingBall(void);

	int Update(void) override;
};