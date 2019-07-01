#pragma once
#include "Application.h"
#include "FVector2.h"
#include "InputHandler.h"

/*
 * Bouncing Ball
 * The app that controls the logic and display of the bouncing ball app.
 */
class BouncingBall : public Application
{
private:
	// Gameplay
	FVector2 position;
	FVector2 direction;
	float speed;

	// Game Logic Functions
	void InputHandler(void) override;
	void GameLogic(void) override;
	void Draw(void) override;

	// Misc Functions
	void GenerateAssets(void) override;

public:
	BouncingBall(RenderEngine* rend, Time* time, int screenWidth, int screenHeight);
	~BouncingBall(void);

	bool Update(void) override;
	void Reset(void) override;
};