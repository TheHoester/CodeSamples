#pragma once
#include <algorithm>
#include <chrono>
#include <vector>
#include <Windows.h>
#include "Application.h"
#include "FVector2.h"
#include "InputHandler.h"

using namespace std;

/**
 * FirstPerson
 * The app that controls the logic and display of the first person game.
 */
class FirstPerson : public Application
{
private:
	// Assets
	wstring map;

	// Gameplay
	const float fov = 3.14159f / 4.0f;
	const float depthOfField = 16.0f;

	FVector2 player;
	FVector2 direction;
	FVector2 moveVelocity;
	float playerA;

	const float moveSpeed;
	const float rotationSpeed;

	// Game Logic Functions
	void InputHandler(void) override;
	void GameLogic(void) override;
	void Draw(void) override;

	// Misc Functions
	void GenerateAssets(void) override;

public:
	FirstPerson(RenderEngine* rend, Time* time, int screenWidth, int screenHeight);
	FirstPerson(RenderEngine* rend, Time* time, int screenWidth, int screenHeight, int fieldWidth, int fieldHeight, wstring map);
	~FirstPerson(void);

	bool Update(void) override;
	void Reset(void) override;
};