#include "BouncingBall.h"

/**
 * Constructor
 * @param rend The render engine used to draw to the console.
 * @param time The time object that is used to track the length of time between frames.
 * @param screenWidth Character width of the screen.
 * @param screenHeight Character height of the screen.
 */
BouncingBall::BouncingBall(RenderEngine* rend, Time* time, int screenWidth, int screenHeight) :
	Application(rend, time, screenWidth, screenHeight), position(screenWidth / 2, screenHeight / 2), direction(1.0f, 1.0f), speed(10.0f)
{
	keys = new bool[1];

	GenerateAssets();
}

/**
 * Destructor
 */
BouncingBall::~BouncingBall() { }

/**
 * Update()
 * Handles running all the logic for the game.
 * @return The current gameover state.
 */
bool BouncingBall::Update()
{
	InputHandler();
	GameLogic();
	Draw();

	return gameOver;
}

/**
 * Reset()
 * Resets the game back to it's starting state.
 */
void BouncingBall::Reset()
{
	Application::Reset();
}

/**
 * InputHandler()
 * Handles getting the current state of all the input.
 */
void BouncingBall::InputHandler()
{
	keys[0] = InputHandler::IsKeyDown(VK_BACK);
}

/**
 * GameLogic()
 * Handles the changing of state depending on the input given.
 */
void BouncingBall::GameLogic()
{
	gameOver = keys[0];

	position += (direction * speed * time->GetDeltaTime());

	// Boundary Check
	if (position.x < 3.0f)
	{
		position.x = 3.0f;
		direction.x *= -1;
	}
	else if (position.x > screenWidth - 3.0f)
	{
		position.x = screenWidth - 3.0f;
		direction.x *= -1;
	}

	if (position.y < 3.0f)
	{
		position.y = 3.0f;
		direction.y *= -1;
	}
	else if (position.y > screenHeight - 3.0f)
	{
		position.y = screenHeight - 3.0f;
		direction.y *= -1;
	}
}

/**
 * Draw()
 * Handles drawing the game to the console.
 */
void BouncingBall::Draw()
{
	// Draw Field
	for (int x = 0; x < screenWidth; ++x)
		for (int y = 0; y < screenHeight; ++y)
			screen[(y * screenWidth) + x] = L" #"[field[(y * screenWidth) + x]];

	// Draw Ball
	for (int x = -2; x <= 2; ++x)
		for (int y = -2; y <= 2; ++y)
			if (!((x == -2 || x == 2) && (y == -2 || y == 2)))
				screen[(((int)position.y + y) * screenWidth) + (int)position.x + x] = 0x2588;


	renderer->Draw(screen);
}

/**
 * GenerateAssets()
 * Used to populate all variables with the assets needed to draw the game.
 * Usually called in the constructor and the reset function.
 */
void BouncingBall::GenerateAssets()
{
	field = Application::GenerateFieldBox(screenWidth, screenHeight, 1);
}