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
	short colour;

	// Gameplay
	FVector2 position;
	FVector2 velocity;
	FVector2 acceleration;
	float force;
	float mass;
	int state;

	// Game Logic Functions
	void GameLogic(void);
	void Draw(void);
	void Reset(void);

	// Misc Functions
	void GenerateAssets(void) override;

public:
	BouncingBall(GameEngine* engine, int appID, int width = 240, int height = 120, int fontWidth = 4, int fontHeight = 4, int radius = 7);
	~BouncingBall(void);

	int Update(void) override;
};