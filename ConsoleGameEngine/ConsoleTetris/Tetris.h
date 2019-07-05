#pragma once
#include <string>
#include <vector>

#include "Application.h"
#include "GameEngine.h"
#include "InputHandler.h"

using namespace std;

/**
 * Tetris
 * The app that controls the logic and display of the tetris game.
 */
class Tetris : public Application
{
private:
	// Assets
	unsigned char* field;
	wstring tetrominos[7];
	short tetroColours[7];
	const int assetWidth = 4;
	const int fieldWidth = 12;
	const int fieldHeight = 18;

	// Current piece variables
	int currentPiece;
	int currentRotation;
	int currentX;
	int currentY;

	// Controls
	float inputDelay;
	float inputCounter;
	bool canInput;

	// Gameplay
	float movementDelay;
	float movementCounter;
	bool forceDown;
	int pieceCount;
	vector<int> lines;
	int score;
	bool gameOver;

	// Game Logic Functions
	void GameLogic(void) override;
	void Draw(void) override;
	void Reset(void);

	// Misc Functions
	void GenerateAssets(void) override;
	int Rotate(const int& posX, const int& posY, const int& rotation);
	bool DoesPieceFit(const int& tetromino, const int& rotation, const int& posX, const int& posY);
		
public:
	Tetris(CHAR_INFO* screenBuffer, InputHandler* input, Time* time, int appID, int width = 80, int height = 30, int fontWidth = 8, int fontHeight = 16);
	~Tetris(void);

	int Update(void) override;
};