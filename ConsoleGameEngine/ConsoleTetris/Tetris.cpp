#include "Tetris.h"

/**
 * Constructor
 * @param rend The render engine used to draw to the console.
 * @param time The time object that is used to track the length of time between frames.
 * @param screenWidth Character width of the screen.
 * @param screenHeight Character height of the screen.
 * @param fieldWidth Character width of the play field.
 * @param fieldHeight Character height of the play field.
 */
Tetris::Tetris(GameEngine* engine, int appID, int width, int height, int fontWidth, int fontHeight) : Application(engine, appID, width, height, fontWidth, fontHeight),
	currentPiece(rand() % 7), currentRotation(0), currentX(fieldWidth / 2), currentY(0), inputDelay(0.075f), inputCounter(0.075f), canInput(false), 
	movementDelay(1.0f), movementCounter(0.0f), forceDown(false), pieceCount(0), gameOver(false) 
{ 
	GenerateAssets();
}

/**
 * Destructor
 */
Tetris::~Tetris() 
{
	if (field != NULL)
		delete[] field;
}


int Tetris::Update()
{
	if (!gameOver)
	{
		GameLogic();
		Draw();
	}
	else
	{
		if (InputHandler::Instance().IsKeyPressed(VK_RETURN))
			Reset();
		else if (InputHandler::Instance().IsKeyPressed(VK_ESCAPE))
		{
			Reset();
			return 0;
		}
	}

	return appID;
}

void Tetris::Reset()
{
	if (field != NULL)
		delete[] field;

	field = new unsigned char[fieldWidth * fieldHeight];
	for (int x = 0; x < fieldWidth; ++x)
		for (int y = 0; y < fieldHeight; ++y)
			field[(y * fieldWidth) + x] = (x == 0 || x == fieldWidth - 1 || y == fieldHeight - 1) ? 9 : 0;

	currentPiece = rand() % 7;
	currentRotation = 0;
	currentX = fieldWidth / 2;
	currentY = 0;

	inputCounter = 0.075f;
	canInput = false;

	movementDelay = 1.0f;
	movementCounter = 0.0f;
	forceDown = false;
	pieceCount = 0;
	lines.clear();
	score = 0;
	gameOver = false;

	engine->ClearScreen();
}

/**
 * GameLogic()
 * Handles the changing of state depending on the input given.
 */
void Tetris::GameLogic()
{
	if (inputCounter < inputDelay)
		inputCounter += Time::Instance().DeltaTime();
	canInput = (inputCounter >= inputDelay);

	bool check = false;

	if (InputHandler::Instance().IsKeyHeld(VK_RIGHT)) check = true;
	else if (InputHandler::Instance().IsKeyHeld(VK_LEFT)) check = true;
	else if (InputHandler::Instance().IsKeyHeld(VK_DOWN)) check = true;

	if (!check)
	{
		inputCounter = inputDelay;
		canInput = true;
	}
	else if (check && canInput)
		inputCounter = 0.0f;

	movementCounter += Time::Instance().DeltaTime();
	forceDown = (movementCounter >= movementDelay);

	// Input Handling
	if (canInput)
	{
		currentX += (InputHandler::Instance().IsKeyHeld(VK_RIGHT) && DoesPieceFit(currentPiece, currentRotation, currentX + 1, currentY)) ? 1 : 0; // Right Key Pressed
		currentX -= (InputHandler::Instance().IsKeyHeld(VK_LEFT) && DoesPieceFit(currentPiece, currentRotation, currentX - 1, currentY)) ? 1 : 0; // Left Key Pressed
		currentY += (InputHandler::Instance().IsKeyHeld(VK_DOWN) && DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1)) ? 1 : 0; // Down Key Pressed
	}

	currentRotation += (InputHandler::Instance().IsKeyPressed('Z') && DoesPieceFit(currentPiece, currentRotation + 1, currentX, currentY)) ? 1 : 0; // Rotation Key Pressed

	// Falling Physics
	if (forceDown)
	{
		if (DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1))
			++currentY;
		else
		{
			// Lock current piece in place
			for (int x = 0; x < assetWidth; ++x)
				for (int y = 0; y < assetWidth; ++y)
					if (tetrominos[currentPiece][Rotate(x, y, currentRotation)] == L'X')
						field[((currentY + y) * fieldWidth) + (currentX + x)] = currentPiece + 1;

			++pieceCount;
			if (pieceCount % 10 == 0 && movementDelay >= 0.1f)
				movementDelay -= 0.1f;

			// Line Detection
			for (int y = 0; y < assetWidth; ++y)
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
		movementCounter = 0.0f;
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
	{
		for (int y = 0; y < fieldHeight; ++y)
		{
			switch (field[(y * fieldWidth) + x])
			{
			case 0:
				engine->DrawChar(x + 2, y + 2, ' ');
				break;
			case 8: case 9:
				engine->DrawChar(x + 2, y + 2);
				break;
			default:
				engine->DrawChar(x + 2, y + 2, PIXEL_SOLID, tetroColours[field[(y * fieldWidth) + x] - 1]);
			}
		}
	}

	// Draw Current Piece
	for (int x = 0; x < assetWidth; ++x)
		for (int y = 0; y < assetWidth; ++y)
			if (tetrominos[currentPiece][Rotate(x, y, currentRotation)] == L'X')
				engine->DrawChar(currentX + x + 2, currentY + y + 2, PIXEL_SOLID, tetroColours[currentPiece]);

	// Draw Score
	std::wstring scoreText = L"SCORE: " + std::to_wstring(score);
	engine->DrawString(fieldWidth + 6, 2, scoreText, FG_WHITE);

	// Gameover screen
	if (gameOver)
	{
		engine->DrawString(fieldWidth + 6, 4, L"GAME OVER!!", FG_WHITE);
		engine->DrawString(fieldWidth + 6, 5, L"Press [ENTER] to retry", FG_WHITE);
		engine->DrawString(fieldWidth + 6, 6, L"Press [ESCAPE] to exit", FG_WHITE);
	}

	// Line Completion
	if (!lines.empty())
	{
		// TODO Remove sleep for a timer
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
}

/*
 * GenerateAssets()
 * Initilizes all the assets for the game.
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

	tetroColours[0] = FG_BLUE;
	tetroColours[1] = FG_RED;
	tetroColours[2] = FG_YELLOW;
	tetroColours[3] = FG_GREEN;
	tetroColours[4] = FG_MAGENTA;
	tetroColours[5] = FG_CYAN;
	tetroColours[6] = FG_DARK_RED;

	field = new unsigned char[fieldWidth * fieldHeight];
	for (int x = 0; x < fieldWidth; ++x)
		for (int y = 0; y < fieldHeight; ++y)
			field[(y * fieldWidth) + x] = (x == 0 || x == fieldWidth - 1 || y == fieldHeight - 1) ? 9 : 0;
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
int Tetris::Rotate(const int& posX, const int& posY, const int& rotation)
{
	switch (rotation % 4)
	{
	case 0:
		return (posY * assetWidth) + posX; // 0 degrees
	case 1:
		return (12 + posY) - (posX * assetWidth); // 90 degrees
	case 2:
		return 15 - (posY * assetWidth) - posX; // 180 degrees
	case 3:
		return 3 - posY + (posX * assetWidth); // 270 degrees
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
bool Tetris::DoesPieceFit(const int& tetromino, const int& rotation, const int& posX, const int& posY)
{
	for (int x = 0; x < assetWidth; ++x)
	{
		for (int y = 0; y < assetWidth; ++y)
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