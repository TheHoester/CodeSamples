#include "GameEngine.h"

/*
 * Constructor
 * @param name The name that will be displayed on the top bar.
 * @param width Character width of the screen.
 * @param height Character height of the screen.
 * @param width Pixel width of the font.
 * @param height Pixel height of the font.
 */
Engine::GameEngine::GameEngine(std::wstring name, int width, int height, int fontWidth, int fontHeight) : appName(name), screenWidth(width), screenHeight(height)
{
	screenBuffer = new CHAR_INFO[screenWidth * screenHeight];
	memset(screenBuffer, 0, sizeof(CHAR_INFO) * screenWidth * screenHeight);

	renderer = new RenderEngine(screenWidth, screenHeight, fontWidth, fontHeight);
	inputHandler = new InputHandler();
	time = new Time();

	close = false;
}

/*
 * Destructor
 */
Engine::GameEngine::~GameEngine()
{
	if (screenBuffer != NULL)
		delete[] screenBuffer;
	if (renderer != NULL)
		delete renderer;
	if (inputHandler != NULL)
		delete inputHandler;
	if (time != NULL)
		delete time;
}

// ENGINE FUNCTIONS ##########################################################################################################################################

/*
 * Start()
 * Used to set the game engine running.
 */
void Engine::GameEngine::Start()
{
	std::thread mainThread = std::thread(&Engine::GameEngine::ThreadUpdate, this);
	mainThread.join();
}

/*
 * ThreadUpdate()
 * Initializes the game and sets the main game loop running.
 */
void Engine::GameEngine::ThreadUpdate()
{
	// Generate Assets
	CreateGame();

	// Main Game Loop
	while (!close)
	{
		inputHandler->UpdateKeyState();
		time->Update();
		RunGame();
		renderer->Draw(appName.c_str(), screenBuffer);
	}
}

// DRAWING FUNCTIONS #########################################################################################################################################

/*
 * ClearScreen()
 * Clears the screen buffer with empty spaces to clear the screen.
 */
void Engine::GameEngine::ClearScreen()
{
	for (int i = 0; i < screenWidth * screenHeight; ++i)
	{
		screenBuffer[i].Char.UnicodeChar = ' ';
		screenBuffer[i].Attributes = FG_WHITE;
	}
}

/*
 * DrawChar()
 * Will change a character and its colour in the screen buffer at the specified coordinates.
 * @param x The x coordinate of the character to set.
 * @param y The y coordinate of the character to set.
 * @param character What the character should be.
 * @param colour The new colour of the character.
 */
void Engine::GameEngine::DrawChar(const int& x, const int& y, const short& character, const short& colour)
{
	// TODO: Error check
	if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
	{
		screenBuffer[(y * screenWidth) + x].Char.UnicodeChar = character;
		screenBuffer[(y * screenWidth) + x].Attributes = colour;
	}
}

/*
 * DrawRect()
 * Draws the outline of a rectangle that will map to the top left and bottom right coordinates given.
 * @param minX The x coordinate of the top left corner of the rectangle.
 * @param minY The y coordinate of the top left corner of the rectangle.
 * @param maxX The x coordinate of the bottom right corner of the rectangle.
 * @param maxY The y coordinate of the bottom right corner of the rectangle.
 * @param character What the rectangles character should be.
 * @param colour The new colour of the rectangle.
 */
void Engine::GameEngine::DrawRect(const int& minX, const int& minY, const int& maxX, const int& maxY, const short& character, const short& colour)
{
	// TODO: Error check
	if (minX >= 0 && maxX < screenWidth && minY >= 0 && maxY < screenHeight && minX <= maxX && minY <= maxY)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			for (int y = minY; y <= maxY; ++y)
			{
				if (x == minX || y == minY || x == maxX || y == maxY)
				{
					screenBuffer[(y * screenWidth) + x].Char.UnicodeChar = character;
					screenBuffer[(y * screenWidth) + x].Attributes = colour;
				}
			}
		}
	}
}

/*
 * DrawRectFill()
 * Draws a rectangle with a border and a fill that will map to the top left and bottom right coordinates given.
 * @param minX The x coordinate of the top left corner of the rectangle.
 * @param minY The y coordinate of the top left corner of the rectangle.
 * @param maxX The x coordinate of the bottom right corner of the rectangle.
 * @param maxY The y coordinate of the bottom right corner of the rectangle.
 * @param borderCharacter What the character of the border should be.
 * @param borderColour The new colour of the border.
 * @param fillCharacter What the character of the fill should be.
 * @param fillColour The new colour of the fill.
 */
void Engine::GameEngine::DrawRectFill(const int& minX, const int& minY, const int& maxX, const int& maxY, const short& borderCharacter, const short& borderColour, const short& fillCharacter, const short& fillColour)
{
	// TODO: Error check
	if (minX >= 0 && maxX < screenWidth && minY >= 0 && maxY < screenHeight && minX <= maxX && minY <= maxY)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			for (int y = minY; y <= maxY; ++y)
			{
				screenBuffer[(y * screenWidth) + x].Char.UnicodeChar = (x == minX || y == minY || x == maxX || y == maxY) ? borderCharacter : fillCharacter;
				screenBuffer[(y * screenWidth) + x].Attributes = (x == minX || y == minY || x == maxX || y == maxY) ? borderColour : fillColour;
			}
		}
	}
}

/*
 * DrawString()
 * Will print a string on screen starting at the coordinates given.
 * @param x The x coordinate of the start of the string.
 * @param y The y coordinate of the start of the string.
 * @param msg The string to be printed on screen.
 * @param colour The new colour of the string.
 */
void Engine::GameEngine::DrawString(const int& x, const int& y, const std::wstring& msg, const short& colour)
{
	// TODO: Error check
	if (x >= 0 && x < screenWidth - msg.size() && y >= 0 && y < screenHeight)
		for (int i = 0; i < msg.size(); i++)
			DrawChar(x + i, y, msg[i], colour);
}

/*
 * DrawCircle()
 * Will change a character and its colour in the screen buffer at the specified coordinates.
 * @param centreX The x coordinate of the centre of the circle.
 * @param centreY The y coordinate of the centre of the circle.
 * @param radius The radius of the circle (Does not include the centre pixel).
 * @param character What the character should be for the circle.
 * @param colour The new colour of the circle.
 */
void Engine::GameEngine::DrawCircle(const int& centreX, const int& centreY, const int& radius, const short& character, const short& colour)
{
	// TODO: Error check
	float fRadius = (float)radius + 0.5f;
	for (int x = radius * -1; x <= radius; ++x)
		for (int y = radius * -1; y <= radius; ++y)
			if (FVector2((float)x, (float)y).Magnitude() < fRadius)
				DrawChar(centreX + x, centreY + y, character, colour);
}

// STATIC DRAWING FUNCTIONS ##################################################################################################################################

/*
 * ClearScreen()
 * Clears the screen buffer with empty spaces to clear the screen.
 * @param buffer The screen buffer to draw to.
 * @param width Width of the screen in characters.
 * @param height Height of the screen in characters.
 */
void Engine::GameEngine::ClearScreen(CHAR_INFO* buffer, const int& width, const int& height)
{
	for (int i = 0; i < width * height; ++i)
	{
		buffer[i].Char.UnicodeChar = ' ';
		buffer[i].Attributes = FG_WHITE;
	}
}

/*
 * DrawChar()
 * Will change a character and its colour in the screen buffer at the specified coordinates.
 * @param buffer The screen buffer to draw to.
 * @param width Width of the screen in characters.
 * @param height Height of the screen in characters.
 * @param x The x coordinate of the character to set.
 * @param y The y coordinate of the character to set.
 * @param character What the character should be.
 * @param colour The new colour of the character.
 */
void Engine::GameEngine::DrawChar(CHAR_INFO* buffer, const int& width, const int& height, const int& x, const int& y, const short& character, const short& colour)
{
	// TODO: Error check
	if (x >= 0 && x < width && y >= 0 && y < height)
	{
		buffer[(y * width) + x].Char.UnicodeChar = character;
		buffer[(y * width) + x].Attributes = colour;
	}
}

/*
 * DrawRect()
 * Draws the outline of a rectangle that will map to the top left and bottom right coordinates given.
 * @param buffer The screen buffer to draw to.
 * @param width Width of the screen in characters.
 * @param height Height of the screen in characters.
 * @param minX The x coordinate of the top left corner of the rectangle.
 * @param minY The y coordinate of the top left corner of the rectangle.
 * @param maxX The x coordinate of the bottom right corner of the rectangle.
 * @param maxY The y coordinate of the bottom right corner of the rectangle.
 * @param character What the rectangles character should be.
 * @param colour The new colour of the rectangle.
 */
void Engine::GameEngine::DrawRect(CHAR_INFO* buffer, const int& width, const int& height, const int& minX, const int& minY, const int& maxX, const int& maxY, const short& character, const short& colour)
{
	// TODO: Error check
	if (minX >= 0 && maxX < width && minY >= 0 && maxY < height && minX <= maxX && minY <= maxY)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			for (int y = minY; y <= maxY; ++y)
			{
				if (x == minX || y == minY || x == maxX || y == maxY)
				{
					buffer[(y * width) + x].Char.UnicodeChar = character;
					buffer[(y * width) + x].Attributes = colour;
				}
			}
		}
	}
}

/*
 * DrawRectFill()
 * Draws a rectangle with a border and a fill that will map to the top left and bottom right coordinates given.
 * @param buffer The screen buffer to draw to.
 * @param width Width of the screen in characters.
 * @param height Height of the screen in characters.
 * @param minX The x coordinate of the top left corner of the rectangle.
 * @param minY The y coordinate of the top left corner of the rectangle.
 * @param maxX The x coordinate of the bottom right corner of the rectangle.
 * @param maxY The y coordinate of the bottom right corner of the rectangle.
 * @param borderCharacter What the character of the border should be.
 * @param borderColour The new colour of the border.
 * @param fillCharacter What the character of the fill should be.
 * @param fillColour The new colour of the fill.
 */
void Engine::GameEngine::DrawRectFill(CHAR_INFO* buffer, const int& width, const int& height, const int& minX, const int& minY, const int& maxX, const int& maxY, const short& borderCharacter, const short& borderColour, const short& fillCharacter, const short& fillColour)
{
	// TODO: Error check
	if (minX >= 0 && maxX < width && minY >= 0 && maxY < height && minX <= maxX && minY <= maxY)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			for (int y = minY; y <= maxY; ++y)
			{
				buffer[(y * width) + x].Char.UnicodeChar = (x == minX || y == minY || x == maxX || y == maxY) ? borderCharacter : fillCharacter;
				buffer[(y * width) + x].Attributes = (x == minX || y == minY || x == maxX || y == maxY) ? borderColour : fillColour;
			}
		}
	}
}

/*
 * DrawString()
 * Will print a string on screen starting at the coordinates given.
 * @param buffer The screen buffer to draw to.
 * @param width Width of the screen in characters.
 * @param height Height of the screen in characters.
 * @param x The x coordinate of the start of the string.
 * @param y The y coordinate of the start of the string.
 * @param msg The string to be printed on screen.
 * @param colour The new colour of the string.
 */
void Engine::GameEngine::DrawString(CHAR_INFO* buffer, const int& width, const int& height, const int& x, const int& y, const std::wstring& msg, const short& colour)
{
	// TODO: Error check
	if (x >= 0 && x < width - msg.size() && y >= 0 && y < height)
	{
		for (int i = 0; i < msg.size(); i++)
		{
			buffer[(y * width) + x + i].Char.UnicodeChar = msg[i];
			buffer[(y * width) + x + i].Attributes = colour;
		}
	}
}

/*
 * DrawCircle()
 * Will change a character and its colour in the screen buffer at the specified coordinates.
 * @param buffer The screen buffer to draw to.
 * @param width Width of the screen in characters.
 * @param height Height of the screen in characters.
 * @param centreX The x coordinate of the centre of the circle.
 * @param centreY The y coordinate of the centre of the circle.
 * @param radius The radius of the circle (Does not include the centre pixel).
 * @param character What the character should be for the circle.
 * @param colour The new colour of the circle.
 */
void Engine::GameEngine::DrawCircle(CHAR_INFO* buffer, const int& width, const int& height, const int& centreX, const int& centreY, const int& radius, const short& character, const short& colour)
{
	// TODO: Error check
	float fRadius = (float)radius + 0.5f;
	for (int x = radius * -1; x <= radius; ++x)
	{
		for (int y = radius * -1; y <= radius; ++y)
		{
			if (FVector2((float)x, (float)y).Magnitude() < fRadius)
			{
				buffer[((centreY + y) * width) + centreX + x].Char.UnicodeChar = character;
				buffer[((centreY + y) * width) + centreX + x].Attributes = colour;
			}
		}
	}
}