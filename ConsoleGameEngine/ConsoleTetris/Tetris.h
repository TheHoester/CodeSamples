#pragma once
#include <thread>
#include <vector>
#include <Windows.h>

#include "Application.h"
#include "InputHandler.h"
#include "RenderEngine.h"

using namespace std;

/**
 * Tetris
 * The app that controls the logic and display of the tetris game.
 */
class Tetris : public Application
{
private:
	// Assets
	wstring tetrominos[7];
	static const int ASSET_WIDTH = 4;

	// Current piece variables
	int currentPiece;
	int currentRotation;
	int currentX;
	int currentY;

	// Controls
	bool rotateHold;

	// Gameplay
	int speed;
	int speedCounter;
	bool forceDown;
	int pieceCount;
	vector<int> lines;

	// Game Logic Functions
	void InputHandler(void) override;
	void GameLogic(void) override;
	void Draw(void) override;

	// Misc Functions
	void GenerateAssets(void) override;
	int Rotate(int posX, int posY, int rotation);
	bool DoesPieceFit(int tetromino, int rotation, int posX, int posY);
		
public:
	Tetris(RenderEngine* rend, int screenWidth, int screenHeight, int fieldWidth, int fieldHeight);
	~Tetris(void);

	bool Update(void) override;
	void Reset(void) override;
};