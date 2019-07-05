#pragma once
#include <algorithm>
#include <string>
#include <vector>

#include "Application.h"
#include "Colour.h"
#include "Defines.h"
#include "GameEngine.h"
#include "InputHandler.h"

/**
 * FirstPerson
 * The app that controls the logic and display of the first person game.
 */
class FirstPerson : public Application
{
private:
	// Assets
	const int mapWidth;
	const int mapHeight;
	std::wstring map;

	// Gameplay
	const float fov = PI / 4.0f;
	const float depthOfField = 16.0f;
	const float moveSpeed = 5.0f;
	const float rotationSpeed = 1.5f;

	FVector2 player;
	FVector2 direction;
	FVector2 moveVelocity;
	float playerA;

	// Game Logic Functions
	void GameLogic(void) override;
	void Draw(void) override;
	void Reset(void);

	// Misc Functions
	void GenerateAssets(void) override;

public:
	FirstPerson(CHAR_INFO* screenBuffer, InputHandler* input, Time* time, int appID, int width = 320, int height = 160, int fontWidth = 4, int fontHeight = 4);
	FirstPerson(CHAR_INFO* screenBuffer, InputHandler* input, Time* time, int appID, int width, int height, int fontWidth, int fontHeight, 
		int mapWidth, int mapHeight, std::wstring map);
	~FirstPerson(void);

	int Update(void) override;
};