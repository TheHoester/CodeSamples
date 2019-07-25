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
BouncingBall::BouncingBall(GameEngine* engine, int appID, int width, int height, int fontWidth, int fontHeight, int radius) : Application(engine, appID, width, height, fontWidth, fontHeight),
	radius(radius), colour(1), position(screenWidth / 2, screenHeight / 2), velocity(60.0f, 60.0f), acceleration(0.0f, 0.0f), force(10.0f), mass(1.0f), state(1) { }

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

	if (InputHandler::Instance().IsKeyPressed(VK_RETURN))
		Reset();
	if (InputHandler::Instance().IsKeyPressed(VK_ESCAPE))
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
	if (InputHandler::Instance().IsKeyPressed('1'))
		state = 1;
	if (InputHandler::Instance().IsKeyPressed('2'))
		state = 2;
	if (InputHandler::Instance().IsKeyPressed('3'))
	{
		state = 3;
		velocity = FVector2(0.0f, 0.0f);
	}

	if (state == 1)
	{
		if (InputHandler::Instance().IsMouseButtonPressed(0))
		{
			++colour;
			if (colour >= 16)
				colour = 1;
		}

		position.x = InputHandler::Instance().GetMousePosition().x;
		position.y = InputHandler::Instance().GetMousePosition().y;
	}
	else if (state == 2)
	{
		acceleration *= 0.0f;
		if (InputHandler::Instance().IsMouseButtonPressed(0))
		{
			FVector2 direction = (position - InputHandler::Instance().GetMousePosition()).Normalized();
			acceleration += direction * force;
		}

		velocity += acceleration;
		position += (velocity * Time::Instance().DeltaTime());

		// Boundary Check
		if (position.x < radius + 1.0f)
		{
			position.x = radius + 1.0f;
			velocity.x *= -1;
		}
		else if (position.x > screenWidth - radius - 1.0f)
		{
			position.x = screenWidth - radius - 1.0f;
			velocity.x *= -1;
		}

		if (position.y < radius + 1.0f)
		{
			position.y = radius + 1.0f;
			velocity.y *= -1;
		}
		else if (position.y > screenHeight - radius - 1.0f)
		{
			position.y = screenHeight - radius - 1.0f;
			velocity.y *= -1;
		}
	}
	else if (state == 3)
	{
		FVector2 direction = (InputHandler::Instance().GetMousePosition() - position).Normalized();
		acceleration = direction * 0.75f;
		velocity += acceleration;
		position += velocity * Time::Instance().DeltaTime();
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
				engine->DrawChar(x, y, PIXEL_SOLID, FG_DARK_BLUE);
			else
				engine->DrawChar(x, y, ' ');
		}
	}

	// Draw Ball
	engine->DrawCircle(position.x, position.y, radius, PIXEL_SOLID, colour);
}

/**
 * ResetGame()
 * Resets the game back to it's starting state.
 */
void BouncingBall::Reset()
{
	position = FVector2(screenWidth / 2, screenHeight / 2);
	velocity = FVector2(60.0f, 60.0f);
}

void BouncingBall::GenerateAssets() { }