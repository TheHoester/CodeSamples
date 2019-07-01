#include "Snake.h"

/**
 * Constructor
 * @param rend The render engine used to draw to the console.
 * @param time The time object that is used to track the length of time between frames.
 * @param screenWidth Character width of the screen.
 * @param screenHeight Character height of the screen.
 * @param fieldWidth Character width of the play field.
 * @param fieldHeight Character height of the play field.
 */
Snake::Snake(RenderEngine* rend, Time* time, int screenWidth, int screenHeight, int fieldWidth, int fieldHeight) : 
	Application(rend, time, screenWidth, screenHeight), currentDirection(3), movementDelay(0.3f), movementCounter(0.0f), move(false)
{
	Application::fieldWidth = fieldWidth;
	Application::fieldHeight = fieldHeight;

	GenerateAssets();

	keys = new bool[4];
}

/**
 * Destructor
 */
Snake::~Snake() { }

/**
 * Update()
 * Handles running all the logic for the game.
 * @return The current gameover state.
 */
bool Snake::Update()
{
	movementCounter += time->GetDeltaTime();
	move = (movementCounter >= movementDelay);

	InputHandler();
	GameLogic();
	Draw();

	return gameOver;
}

/**
 * Reset()
 * Resets the game back to it's starting state.
 */
void Snake::Reset()
{
	// Delete Old Assets
	Application::Reset();
	while (!snake.empty())
		snake.pop();

	// Create New Assets
	GenerateAssets();

	currentDirection = 3;
	movementCounter = 0.0f;
	move = false;
}

/**
 * InputHandler()
 * Handles getting the current state of all the input.
 */
void Snake::InputHandler()
{
	keys[0] = InputHandler::IsKeyDown(VK_RIGHT);
	keys[1] = InputHandler::IsKeyDown(VK_LEFT);
	keys[2] = InputHandler::IsKeyDown(VK_DOWN);
	keys[3] = InputHandler::IsKeyDown(VK_UP);
}

/**
 * GameLogic()
 * Handles the changing of state depending on the input given.
 */
void Snake::GameLogic()
{
	// Input Handling
	for (int i = 0; i < 4; ++i)
		if (keys[i])
			currentDirection = i;

	// Movement
	if (move)
	{
		Move(currentDirection);
		movementCounter = 0.0f;
	}
}

/**
 * Draw()
 * Handles drawing the game to the console.
 */
void Snake::Draw()
{
	// Draw Field
	for (int x = 0; x < fieldWidth; ++x)
		for (int y = 0; y < fieldHeight; ++y)
			screen[((y + 2) * screenWidth) + (x + 2)] = L" Oo£#"[field[(y * fieldWidth) + x]];

	// Draw Score
	swprintf_s(&screen[(2 * screenWidth) + fieldWidth + 6], 16, L"SCORE: %8d", score);

	// Gameover screen
	if (gameOver)
	{
		swprintf_s(&screen[(4 * screenWidth) + fieldWidth + 6], 12, L"GAME OVER!!");
		swprintf_s(&screen[(5 * screenWidth) + fieldWidth + 6], 23, L"Press [ENTER] to retry");
		swprintf_s(&screen[(6 * screenWidth) + fieldWidth + 6], 36, L"Press [BACKSPACE] to return to menu");
	}

	// Display Frame
	renderer->Draw(screen);
}

/**
 * GenerateAssets()
 * Used to populate all variables with the assets needed to draw the game.
 * Usually called in the constructor and the reset function.
 */
void Snake::GenerateAssets()
{
	field = Application::GenerateFieldBox(fieldWidth, fieldHeight, 4);

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
void Snake::Move(int direction)
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