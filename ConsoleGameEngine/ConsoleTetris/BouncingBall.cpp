#include "BouncingBall.h"

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
 * @param radus The radius of the ball.
 */
BouncingBall::BouncingBall(CHAR_INFO* screenBuffer, InputHandler* input, Time* time, int appID, int width, int height, int fontWidth, int fontHeight, int radius) :
	Application(screenBuffer, input, time, appID, width, height, fontWidth, fontHeight), radius(radius), position(screenWidth / 2, screenHeight / 2), direction(1.0f, 1.0f), speed(30.0f)
{

}

/**
 * Destructor
 */
BouncingBall::~BouncingBall() 
{ 

}

/*
 * Update()
 * Public call to the main update function for the game.
 * @return 0 is the game should exit to menu, else the game ID number.
 */
int BouncingBall::Update()
{
	GameLogic();
	Draw();

	if (input->IsKeyPressed(VK_RETURN))
		Reset();
	if (input->IsKeyPressed(VK_ESCAPE))
	{
		Reset();
		return 0;
	}

	return appID;
}

/*
 * GameLogic()
 * Runs the main logic for the game.
 */
void BouncingBall::GameLogic()
{
	position += (direction * speed * time->GetDeltaTime());

	// Boundary Check
	if (position.x < radius + 1.0f)
	{
		position.x = radius + 1.0f;
		direction.x *= -1;
	}
	else if (position.x > screenWidth - radius - 1.0f)
	{
		position.x = screenWidth - radius - 1.0f;
		direction.x *= -1;
	}

	if (position.y < radius + 1.0f)
	{
		position.y = radius + 1.0f;
		direction.y *= -1;
	}
	else if (position.y > screenHeight - radius - 1.0f)
	{
		position.y = screenHeight - radius - 1.0f;
		direction.y *= -1;
	}
}

/*
 * Draw()
 * Draws the game to the screen buffer.
 */
void BouncingBall::Draw()
{
	// Draw Field
	for (int x = 0; x < screenWidth; ++x)
	{
		for (int y = 0; y < screenHeight; ++y)
		{
			if (x == 0 || y == 0 || x == screenWidth - 1 || y == screenHeight - 1)
				GameEngine::DrawChar(screenBuffer, screenWidth, screenHeight, x, y, PIXEL_SOLID, FG_DARK_BLUE);
			else
				GameEngine::DrawChar(screenBuffer, screenWidth, screenHeight, x, y, ' ');
		}
	}
	

	// Draw Ball
	GameEngine::DrawCircle(screenBuffer, screenWidth, screenHeight, position.x, position.y, radius, PIXEL_SOLID, FG_RED);
}

/**
 * ResetGame()
 * Resets the game back to it's starting state.
 */
void BouncingBall::Reset()
{
	position = FVector2(screenWidth / 2, screenHeight / 2);
	direction = FVector2(1.0f, 1.0f);
}

void BouncingBall::GenerateAssets() { }