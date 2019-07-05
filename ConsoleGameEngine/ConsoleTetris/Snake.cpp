#include "Snake.h"

/*
 * Constructor
 * @param screenBuffer A pointer to the screenbuffer to be able to draw to.
 * @param input Pointer to the input handler.
 * @param time Pointer to the time handler.
 * @param appID The id of the application.
 * @param width Character width of the screen.
 * @param height Character height of the screen.
 * @param fontWidth Pixel width of the font.
 * @param fontHeight Pixel height of the font.
 */
Snake::Snake(CHAR_INFO* screenBuffer, InputHandler* input, Time* time, int appID, int width, int height, int fontWidth, int fontHeight) :
	Application(screenBuffer, input, time, appID, width, height, fontWidth, fontHeight),
	currentDirection(3), movementCounter(0.0f), move(false), score(0), gameOver(false)
{
	GenerateAssets();
}

/**
 * Destructor
 */
Snake::~Snake() 
{
	if (field != NULL)
		delete[] field;
}

/*
 * Update()
 * Public call to the main update function for the game.
 * @return 0 is the game should exit to menu, else the game ID number.
 */
int Snake::Update()
{
	if (!gameOver)
	{
		GameLogic();
		Draw();
	}
	else
	{
		if (input->IsKeyPressed(VK_RETURN))
			Reset();
		else if (input->IsKeyPressed(VK_ESCAPE))
		{
			Reset();
			return 0;
		}
	}

	return appID;
}

/*
 * GameLogic()
 * Runs the main logic for the game.
 */
void Snake::GameLogic()
{
	movementCounter += time->GetDeltaTime();
	move = (movementCounter >= movementDelay);

	// Input Handling
	if (input->IsKeyPressed(VK_RIGHT)) currentDirection = 0;
	else if (input->IsKeyPressed(VK_LEFT)) currentDirection = 1;
	else if (input->IsKeyPressed(VK_DOWN)) currentDirection = 2;
	else if (input->IsKeyPressed(VK_UP)) currentDirection = 3;

	// Movement
	if (move)
	{
		Move(currentDirection);
		movementCounter = 0.0f;
	}
}

/*
 * Draw()
 * Draws the game to the screen buffer.
 */
void Snake::Draw()
{
	// Draw Field
	for (int x = 0; x < fieldWidth; ++x)
	{
		for (int y = 0; y < fieldHeight; ++y)
		{
			switch (field[(y * fieldWidth) + x])
			{
			case 0:
				GameEngine::DrawChar(screenBuffer, screenWidth, screenHeight, x + 2, y + 2, ' ', BG_DARK_GREY);
				break;
			case 1:
				GameEngine::DrawChar(screenBuffer, screenWidth, screenHeight, x + 2, y + 2, 'O', FG_GREEN | BG_DARK_GREY);
				break;
			case 2:
				GameEngine::DrawChar(screenBuffer, screenWidth, screenHeight, x + 2, y + 2, 'o', FG_GREEN | BG_DARK_GREY);
				break;
			case 3:
				GameEngine::DrawChar(screenBuffer, screenWidth, screenHeight, x + 2, y + 2, '#', FG_YELLOW | BG_DARK_GREY);
				break;
			case 4:
				GameEngine::DrawChar(screenBuffer, screenWidth, screenHeight, x + 2, y + 2);
				break;
			}
		}
	}

	// Draw Score
	std::wstring scoreText = L"SCORE: " + std::to_wstring(score);
	GameEngine::DrawString(screenBuffer, screenWidth, screenHeight, fieldWidth + 6, 2, scoreText, FG_WHITE);

	// Gameover screen
	if (gameOver)
	{
		GameEngine::DrawString(screenBuffer, screenWidth, screenHeight, fieldWidth + 6, 4, L"GAME OVER!!", FG_WHITE);
		GameEngine::DrawString(screenBuffer, screenWidth, screenHeight, fieldWidth + 6, 5, L"Press [ENTER] to retry", FG_WHITE);
		GameEngine::DrawString(screenBuffer, screenWidth, screenHeight, fieldWidth + 6, 6, L"Press [ESCAPE] to exit", FG_WHITE);
	}
}

/*
 * Reset()
 * Resets the game back to the beginning state.
 */
void Snake::Reset()
{
	if (field != NULL)
		delete[] field;

	gameOver = false;
	score = 0;

	while (!snake.empty())
		snake.pop();

	currentDirection = 3;
	movementCounter = 0.0f;
	move = false;

	GenerateAssets();

	GameEngine::ClearScreen(screenBuffer, screenWidth, screenHeight);
}


/*
 * GenerateAssets()
 * Initilizes all the assets for the game.
 */
void Snake::GenerateAssets()
{
	field = new unsigned char[fieldWidth * fieldHeight];
	for (int x = 0; x < fieldWidth; ++x)
		for (int y = 0; y < fieldHeight; ++y)
			field[(y * fieldWidth) + x] = (x == 0 || y == 0 || x == fieldWidth - 1 || y == fieldHeight - 1) ? 4 : 0;

	currentPosition = Vector2(fieldWidth / 2, (fieldHeight / 2) + 1);
	field[(currentPosition.y * fieldWidth) + currentPosition.x] = 2;
	snake.push(currentPosition);
	currentPosition = Vector2(fieldWidth / 2, fieldHeight / 2);
	field[(currentPosition.y * fieldWidth) + currentPosition.x] = 2;
	snake.push(currentPosition);
	currentPosition = Vector2(fieldWidth / 2, (fieldHeight / 2) - 1);
	field[(currentPosition.y * fieldWidth) + currentPosition.x] = 1;
	snake.push(currentPosition);

	NewPellet();
}

/**
 * Move()
 * Will move the snake by one character in a given direction.
 * @param direction The integer representation of the direction the snake should move.
 *					0 = Right | 1 = Left | 2 = Down | 3 = Up
 */
void Snake::Move(const int& direction)
{
	switch (direction)
	{
	case 0:
		++currentPosition.x; // Right
		break;
	case 1:
		--currentPosition.x; // Left
		break;
	case 2:
		++currentPosition.y; // Down
		break;
	case 3:
		--currentPosition.y; // Up
		break;
	}

	if (field[(currentPosition.y * fieldWidth) + currentPosition.x] == 3)
	{
		field[(snake.back().y * fieldWidth) + snake.back().x] = 2;
		field[(currentPosition.y * fieldWidth) + currentPosition.x] = 1;
		snake.push(currentPosition);
		NewPellet();
		score += 25;
	}
	else if (field[(currentPosition.y * fieldWidth) + currentPosition.x] == 0)
	{
		field[(snake.front().y * fieldWidth) + snake.front().x] = 0;
		snake.pop();
		field[(snake.back().y * fieldWidth) + snake.back().x] = 2;
		field[(currentPosition.y * fieldWidth) + currentPosition.x] = 1;
		snake.push(currentPosition);
	}
	else
		gameOver = true;
}

/**
 * NewPellet()
 * Creates a new collectable and places it in a random location.
 */
void Snake::NewPellet()
{
	Vector2 position = Vector2((rand() % (fieldWidth - 2)) + 1, (rand() % (fieldHeight - 2)) + 1);
	while (field[(position.y * fieldWidth) + position.x] != 0)
		position = Vector2((rand() % (fieldWidth - 2)) + 1, (rand() % (fieldHeight - 2)) + 1);

	field[(position.y * fieldWidth) + position.x] = 3;
}