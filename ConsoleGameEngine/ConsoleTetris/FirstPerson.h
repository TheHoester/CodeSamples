#pragma once
#include <algorithm>
#include <list>
#include <string>
#include <vector>

#include "Application.h"
#include "Colour.h"
#include "Defines.h"
#include "GameEngine.h"
#include "InputHandler.h"

struct Object
{
	FVector2 position;
	FVector2 velocity;
	bool remove;
	Sprite* sprite;

};

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
	Sprite* wallSprite;
	Sprite* lampSprite;
	Sprite* fireBallSprite;

	// Gameplay
	const float fov = PI / 4.0f;
	const float depthOfField = 12.0f;
	const float moveSpeed = 5.0f;
	const float rotationSpeed = 1.5f;

	FVector2 player;
	FVector2 direction;
	FVector2 moveVelocity;
	float playerA;

	// World Objects
	std::list<Object> objects;

	float* depthBuffer;

	// Game Logic Functions
	void GameLogic(void) override;
	void Draw(void) override;
	void Reset(void);

	// Misc Functions
	void GenerateAssets(void) override;

public:
	FirstPerson(GameEngine* engine, int appID, int width = 220, int height = 110, int fontWidth = 6, int fontHeight = 6);
	FirstPerson(GameEngine* engine, int appID, int width, int height, int fontWidth, int fontHeight,
		int mapWidth, int mapHeight, std::wstring map);
	~FirstPerson(void);

	int Update(void) override;
};