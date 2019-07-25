#pragma once
#include "Application.h"

class SideScroller : public Application
{
private:
	// Constants
	float GRAVITY = 15.0f;

	// Assets
	Sprite* background1;
	Sprite* ground1;
	Sprite* player1;

	// GameObjects
	GameObject* playerObject;

	// Properties
	float backgroundXPos;
	float playerSpeed;
	FVector2 playerVelocity;
	float jumpForce;
	bool isJumping;

	// Game Logic Functions
	void GameLogic(void);
	void Draw(void);
	void Reset(void);

	// Misc Functions
	void GenerateAssets(void) override;

public:
	SideScroller(GameEngine* engine, int appID, int width = 160, int height = 100, int fontWidth = 6, int fontHeight = 6);
	~SideScroller(void);

	int Update(void) override;
};