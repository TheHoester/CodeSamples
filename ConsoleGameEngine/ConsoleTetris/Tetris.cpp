#include "Tetris.h"

/**
 * Constructor
 * @param rend The render engine used to draw to the console.
 * @param screenWidth Character width of the screen.
 * @param screenHeight Character height of the screen.
 * @param fieldWidth Character width of the play field.
 * @param fieldHeight Character height of the play field.
 */
Tetris::Tetris(RenderEngine* rend, int screenWidth, int screenHeight, int fieldWidth, int fieldHeight) : Application(rend, screenWidth, screenHeight)
{
	Application::fieldWidth = fieldWidth;
	Application::fieldHeight = fieldHeight;

	currentPiece = rand() % 7;
	currentRotation = 0;
	currentX = fieldWidth / 2;
	currentY = 0;

	keys = new bool[4];
	rotateHold = false;

	speed = 20;
	speedCounter = 0;
	forceDown = false;
	pieceCount = 0;

	GenerateAssets();
}

/**
 * Destructor
 */
Tetris::~Tetris() { }

/**
 * Update()
 * Handles running all the logic for the game.
 * @return The current gameover state.
 */
bool Tetris::Update()
{
	this_thread::sleep_for(50ms);
	++speedCounter;
	forceDown = (speedCounter == speed);

	InputHandler();
	GameLogic();
	Draw();

	return gameOver;
}

/**
 * Reset()
 * Resets the game back to it's starting state.
 */
void Tetris::Reset()
{
	// Delete Old Assets
	Application::Reset();

	// Reset Variables
	currentPiece = rand() % 7;
	currentRotation = 0;
	currentX = fieldWidth / 2;
	currentY = 0;

	rotateHold = false;

	speed = 20;
	speedCounter = 0;
	forceDown = false;
	pieceCount = 0;
	lines.clear();

	// Recreate New Assets
	field = Application::GenerateFieldOpenTopBox(fieldWidth, fieldHeight, 9);
}

/**
 * InputHandler()
 * Handles getting the current state of all the input.
 */
void Tetris::InputHandler()
{
	keys[0] = InputHandler::IsKeyDown(VK_RIGHT);
	keys[1] = InputHandler::IsKeyDown(VK_LEFT);
	keys[2] = InputHandler::IsKeyDown(VK_DOWN);
	keys[3] = InputHandler::IsKeyDown('Z');
}

/**
 * GameLogic()
 * Handles the changing of state depending on the input given.
 */
void Tetris::GameLogic()
{
	// Input Handling
	currentX += (keys[0] && DoesPieceFit(currentPiece, currentRotation, currentX + 1, currentY)) ? 1 : 0; // Right Key Pressed
	currentX -= (keys[1] && DoesPieceFit(currentPiece, currentRotation, currentX - 1, currentY)) ? 1 : 0; // Left Key Pressed
	currentY += (keys[2] && DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1)) ? 1 : 0; // Down Key Pressed

	if (keys[3])
	{
		currentRotation += (!rotateHold && DoesPieceFit(currentPiece, currentRotation + 1, currentX, currentY)) ? 1 : 0; // Rotation Key Pressed
		rotateHold = true;
	}
	else
		rotateHold = false;

	// Falling Physics
	if (forceDown)
	{
		if (DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1))
			++currentY;
		else
		{
			// Lock current piece in place
			for (int x = 0; x < ASSET_WIDTH; ++x)
				for (int y = 0; y < ASSET_WIDTH; ++y)
					if (tetrominos[currentPiece][Rotate(x, y, currentRotation)] == L'X')
						field[((currentY + y) * fieldWidth) + (currentX + x)] = currentPiece + 1;

			++pieceCount;
			if (pieceCount % 10 == 0 && speed >= 10)
				--speed;

			// Line Detection
			for (int y = 0; y < ASSET_WIDTH; ++y)
			{
				if (currentY + y < fieldHeight - 1)
				{
					bool line = true;
					for (int x = 1; x < fieldWidth - 1; ++x)
						line &= (field[((currentY + y) * fieldWidth) + x]) != 0;

					if (line)
					{
						for (int x = 1; x < fieldWidth - 1; ++x)
							field[((currentY + y) * fieldWidth) + x] = 8;

						lines.push_back(currentY + y);
					}
				}
			}

			// Update Score
			score += 25;
			if (!lines.empty())
				score += (1 << lines.size()) * 100;

			// Next Piece
			currentX = fieldWidth / 2;
			currentY = 0;
			currentRotation = 0;
			currentPiece = rand() % 7;

			// Gameover State
			gameOver = !DoesPieceFit(currentPiece, currentRotation, currentX, currentY);
		}

		speedCounter = 0;
	}
}

/**
 * Draw()
 * Handles drawing the game to the console.
 */
void Tetris::Draw()
{
	// Draw Field
	for (int x = 0; x < fieldWidth; ++x)
		for (int y = 0; y < fieldHeight; ++y)
			screen[(y + 2) * screenWidth + (x + 2)] = L" ABCDEFG=#"[field[(y * fieldWidth) + x]];

	// Draw Current Piece
	for (int x = 0; x < ASSET_WIDTH; ++x)
		for (int y = 0; y < ASSET_WIDTH; ++y)
			if (tetrominos[currentPiece][Rotate(x, y, currentRotation)] == L'X')
				screen[((currentY + y + 2) * screenWidth) + (currentX + x + 2)] = currentPiece + 65;

	// Draw Score
	swprintf_s(&screen[(2 * screenWidth) + fieldWidth + 6], 16, L"SCORE: %8d", score);

	// Gameover screen
	if (gameOver) 
	{
		swprintf_s(&screen[(4 * screenWidth) + fieldWidth + 6], 12, L"GAME OVER!!");
		swprintf_s(&screen[(5 * screenWidth) + fieldWidth + 6], 23, L"Press [ENTER] to retry");
		swprintf_s(&screen[(6 * screenWidth) + fieldWidth + 6], 36, L"Press [BACKSPACE] to return to menu");
	}

	// Line Completion
	if (!lines.empty())
	{
		renderer->Draw(screen);
		this_thread::sleep_for(400ms);

		for (auto &line : lines)
		{
			for (int x = 1; x < fieldWidth - 1; ++x)
			{
				for (int y = line; y > 0; --y)
					field[(y * fieldWidth) + x] = field[((y - 1) * fieldWidth) + x];

				field[x] = 0;
			}
		}
		lines.clear();
	}

	// Display Frame
	renderer->Draw(screen);
}

/**
 * GenerateAssets()
 * Used to populate all variables with the assets needed to draw the game.
 * Usually called in the constructor and the reset function.
 */
void Tetris::GenerateAssets()
{
	tetrominos[0].append(L"..X.");
	tetrominos[0].append(L"..X.");
	tetrominos[0].append(L"..X.");
	tetrominos[0].append(L"..X.");

	tetrominos[1].append(L"..X.");
	tetrominos[1].append(L".XX.");
	tetrominos[1].append(L".X..");
	tetrominos[1].append(L"....");

	tetrominos[2].append(L".X..");
	tetrominos[2].append(L".XX.");
	tetrominos[2].append(L"..X.");
	tetrominos[2].append(L"....");

	tetrominos[3].append(L"....");
	tetrominos[3].append(L".XX.");
	tetrominos[3].append(L".XX.");
	tetrominos[3].append(L"....");

	tetrominos[4].append(L"..X.");
	tetrominos[4].append(L".XX.");
	tetrominos[4].append(L"..X.");
	tetrominos[4].append(L"....");

	tetrominos[5].append(L"....");
	tetrominos[5].append(L".XX.");
	tetrominos[5].append(L"..X.");
	tetrominos[5].append(L"..X.");

	tetrominos[6].append(L"....");
	tetrominos[6].append(L".XX.");
	tetrominos[6].append(L".X..");
	tetrominos[6].append(L".X..");

	field = Application::GenerateFieldOpenTopBox(fieldWidth, fieldHeight, 9);
}

/**
 * Rotate()
 * Visualizes the array as a 2d array of ASSET_WIDTH using an x and y position to determine its position in the array.
 * Calculates the new position it would be in if the 2d array was rotated by a specified amount. Then returns this 
 * position as an index of a standard array.
 * @param posX The X position within the tetromino asset when not rotated.
 * @param posY The Y position within the tetromino asset when not rotated.
 * @param rotation An integer representation of the current rotation of the tetromino.
 * @return The position in the array from 0 - 15 that the rotated value will be.
 */
int Tetris::Rotate(int posX, int posY, int rotation)
{
	switch (rotation % 4)
	{
	case 0:
		return (posY * ASSET_WIDTH) + posX; // 0 degrees
	case 1:
		return (12 + posY) - (posX * ASSET_WIDTH); // 90 degrees
	case 2:
		return 15 - (posY * ASSET_WIDTH) - posX; // 180 degrees
	case 3:
		return 3 - posY + (posX * ASSET_WIDTH); // 270 degrees
	default:
		return 0;
	}
}

/**
 * Rotate()
 * Compares the position of each section of the tetromino to the field so determine whether there is
 * space for the tetrmino to fit.
 * @param tetromino Index of the tetromino to be checked from the asset array.
 * @param rotation An integer representation of the current rotation of the tetromino.
 * @param posX The X position of the tetromino's position.
 * @param posY The Y position of the tetromino's position.
 * @return True if the piece can fit in the current position given, else false.
 */
bool Tetris::DoesPieceFit(int tetromino, int rotation, int posX, int posY)
{
	for (int x = 0; x < ASSET_WIDTH; ++x)
	{
		for (int y = 0; y < ASSET_WIDTH; ++y)
		{
			int pieceIndex = Rotate(x, y, rotation);
			int fieldIndex = ((posY + y) * fieldWidth) + (posX + x);

			if (posX + x >= 0 && posX + x < fieldWidth)
				if (posY + y >= 0 && posY + y < fieldHeight)
					if (tetrominos[tetromino][pieceIndex] == L'X' && field[fieldIndex] != 0)
						return false;
		}
	}

	return true;
}