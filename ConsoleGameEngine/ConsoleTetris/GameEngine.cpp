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
	RenderEngine::Instance().SetupWindow(width, height, fontWidth, fontHeight);
	Time::Start();
	close = false;
}

/*
 * Destructor
 */
Engine::GameEngine::~GameEngine()
{
	if (screenBuffer != NULL)
		delete[] screenBuffer;

	while (!objectPool.empty())
	{
		delete objectPool.back();
		objectPool.pop_back();
	}
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
		InputHandler::Instance().UpdateKeyState(); // Inputhandler is initialized here - on first loop
		Time::Instance().Update();

		beforeTime = std::chrono::system_clock::now();
		RunGame();
		afterTime = std::chrono::system_clock::now();
		runGameTime = (afterTime - beforeTime).count();

		RenderObjects();

		beforeTime = std::chrono::system_clock::now();
		RenderEngine::Instance().Draw(appName.c_str(), screenBuffer, runGameTime, renderTime);
		afterTime = std::chrono::system_clock::now();
		renderTime = (afterTime - beforeTime).count();
	}
}

// GAMEOBJECT FUNCTIONS ######################################################################################################################################

Engine::GameObject* Engine::GameEngine::CreateGameObject(float x, float y, Sprite* sprite)
{
	GameObject* newObject = new GameObject(x, y, sprite);
	objectPool.push_back(newObject);
	return newObject;
}

Engine::GameObject* Engine::GameEngine::CreateGameObject(FVector2 position,  Sprite* sprite)
{
	GameObject* newObject = new GameObject(position, sprite);
	objectPool.push_back(newObject);
	return newObject;
}

void Engine::GameEngine::RenderObjects()
{
	for (GameObject* object : objectPool)
		if (object->isActive)
			DrawSprite((int)object->screenPosition.x, (int)object->screenPosition.y, *(object->GetSprite()));
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
 * DrawLine()
 * Will draw a line from the start point to the end point using Bresenham's Line Algorithm.
 * @param x0 The x coordinate of the start of the line.
 * @param y0 The y coordinate of the start of the line.
 * @param x1 The x coordinate of the end of the line.
 * @param y1 The y coordinate of the end of the line.
 * @param character What the character should be.
 * @param colour The new colour of the character.
 */
void Engine::GameEngine::DrawLine(int x0,  int y0, const int& x1, const int& y1, const short& character, const short& colour)
{
	// Start and end points are the same.
	if (x0 == x1 && y0 == y1)
	{
		DrawChar(x0, y0, character, colour);
		return;
	}

	// Distance
	int dx = x1 - x0;
	int dy = y1 - y0;

	// Direction
	int sx = (dx < 0) ? -1 : 1;
	int sy = (dy < 0) ? -1 : 1;

	if (abs(dy) < abs(dx))
	{
		float slope = (float)dy / (float)dx;
		float pitch = (float)y0 - slope * (float)x0;
		while (x0 != x1)
		{
			DrawChar(x0, (int)round(slope * x0 + pitch), character, colour);
			x0 += sx;
		}
	}
	else
	{
		float slope = (float)dx / (float)dy;
		float pitch = (float)x0 - slope * (float)y0;
		while (y0 != y1)
		{
			DrawChar((int)round(slope * y0 + pitch), y0, character, colour);
			y0 += sy;
		}
	}
}


/*
 * DrawTriangle()
 * Will draw the outline of a triangle from the 3 points provided.
 * @param x0 The x coordinate of the first of the triangle.
 * @param y0 The y coordinate of the first of the triangle.
 * @param x1 The x coordinate of the second of the triangle.
 * @param y1 The y coordinate of the second of the triangle.
 * @param x2 The x coordinate of the third of the triangle.
 * @param y2 The y coordinate of the third of the triangle.
 * @param character What the character should be.
 * @param colour The new colour of the character.
 */
void Engine::GameEngine::DrawTriangle(const int& x0, const int& y0, const int& x1, const int& y1, const int& x2, const int& y2, const short& character, const short& colour)
{
	DrawLine(x0, y0, x1, y1, character, colour);
	DrawLine(x1, y1, x2, y2, character, colour);
	DrawLine(x2, y2, x0, y0, character, colour);
}

void Engine::GameEngine::DrawFillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const short& character, const short& colour)
{
	auto SWAP = [](int &x, int &y) { int t = x; x = y; y = t; };
	auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; ++i) DrawChar(i, ny, character, colour); };

	int t1x, t2x, y, minx, maxx, t1xp, t2xp;
	bool changed1 = false;
	bool changed2 = false;
	int signx0, signx1, dx0, dy0, dx1, dy1;
	int e1, e2;
	// Sort vertices
	if (y0 > y1) { SWAP(y0, y1); SWAP(x0, x1); }
	if (y0 > y2) { SWAP(y0, y2); SWAP(x0, x2); }
	if (y1 > y2) { SWAP(y1, y2); SWAP(x1, x2); }

	t1x = t2x = x0; y = y0;   // Starting points
	dx0 = (int)(x1 - x0); if (dx0 < 0) { dx0 = -dx0; signx0 = -1; }
	else signx0 = 1;
	dy0 = (int)(y1 - y0);

	dx1 = (int)(x2 - x0); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y2 - y0);

	if (dy0 > dx0) {   // swap values
		SWAP(dx0, dy0);
		changed1 = true;
	}
	if (dy1 > dx1) {   // swap values
		SWAP(dy1, dx1);
		changed2 = true;
	}

	e2 = (int)(dx1 >> 1);
	// Flat top, just process the second half
	if (y0 == y1) goto next;
	e1 = (int)(dx0 >> 1);

	for (int i = 0; i < dx0;) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx0) {
			i++;
			e1 += dy0;
			while (e1 >= dx0) {
				e1 -= dx0;
				if (changed1) t1xp = signx0;//t1x += signx0;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx0;
		}
		// Move line
	next1:
		// process second line until y value is about to change
		while (1) {
			e2 += dy1;
			while (e2 >= dx1) {
				e2 -= dx1;
				if (changed2) t2xp = signx1;//t2x += signx1;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx1;
		}
	next2:
		if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
		drawline(minx, maxx, y);    // Draw line from min to max points found on the y
									 // Now increase y
		if (!changed1) t1x += signx0;
		t1x += t1xp;
		if (!changed2) t2x += signx1;
		t2x += t2xp;
		y += 1;
		if (y == y1) break;

	}
next:
	// Second half
	dx0 = (int)(x2 - x1); if (dx0 < 0) { dx0 = -dx0; signx0 = -1; }
	else signx0 = 1;
	dy0 = (int)(y2 - y1);
	t1x = x1;

	if (dy0 > dx0) {   // swap values
		SWAP(dy0, dx0);
		changed1 = true;
	}
	else changed1 = false;

	e1 = (int)(dx0 >> 1);

	for (int i = 0; i <= dx0; i++) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx0) {
			e1 += dy0;
			while (e1 >= dx0) {
				e1 -= dx0;
				if (changed1) { t1xp = signx0; break; }//t1x += signx0;
				else          goto next3;
			}
			if (changed1) break;
			else   	   	  t1x += signx0;
			if (i < dx0) i++;
		}
	next3:
		// process second line until y value is about to change
		while (t2x != x2) {
			e2 += dy1;
			while (e2 >= dx1) {
				e2 -= dx1;
				if (changed2) t2xp = signx1;
				else          goto next4;
			}
			if (changed2)     break;
			else              t2x += signx1;
		}
	next4:

		if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
		drawline(minx, maxx, y);
		if (!changed1) t1x += signx0;
		t1x += t1xp;
		if (!changed2) t2x += signx1;
		t2x += t2xp;
		y += 1;
		if (y > y2) return;
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
	if (minX <= maxX && minY <= maxY)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			for (int y = minY; y <= maxY; ++y)
			{
				if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
				{
					screenBuffer[(y * screenWidth) + x].Char.UnicodeChar = (x == minX || y == minY || x == maxX || y == maxY) ? borderCharacter : fillCharacter;
					screenBuffer[(y * screenWidth) + x].Attributes = (x == minX || y == minY || x == maxX || y == maxY) ? borderColour : fillColour;
				}
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
	if (x >= 0 && x < (int)(screenWidth - msg.size()) && y >= 0 && y < screenHeight)
		for (int i = 0; i < (int)msg.size(); i++)
			DrawChar(x + i, y, msg[i], colour);
}

/*
 * DrawStringAlpha()
 * Will print a string on screen starting at the coordinates given.
 * All spaces will be ignored.
 * @param x The x coordinate of the start of the string.
 * @param y The y coordinate of the start of the string.
 * @param msg The string to be printed on screen.
 * @param colour The new colour of the string.
 */
void Engine::GameEngine::DrawStringAlpha(const int& x, const int& y, const std::wstring& msg, const short& colour)
{
	// TODO: Error check
	if (x >= 0 && x < (int)(screenWidth - msg.size()) && y >= 0 && y < screenHeight)
		for (int i = 0; i < (int)msg.size(); i++)
			if (msg[i] != ' ')
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

void Engine::GameEngine::DrawSprite(const int& x, const int& y, const Sprite& sprite)
{
	for (int spriteX = 0; spriteX < sprite.Width(); ++spriteX)
		for (int spriteY = 0; spriteY < sprite.Height(); ++spriteY)
			if (x + spriteX >= 0 && x + spriteX < screenWidth && y + spriteY >= 0 && spriteY < screenHeight)
				if (sprite.GetPixel(spriteX, spriteY) != 0)
					DrawChar(x + spriteX, y + spriteY, sprite.GetPixel(spriteX, spriteY), sprite.GetColour(spriteX, spriteY));
}

void Engine::GameEngine::DrawPartialSprite(const int& x, const int& y, const int& minX, const int& minY, const int& maxX, const int& maxY, const Sprite& sprite)
{
	for (int spriteX = 0; spriteX <= maxX - minX; ++spriteX)
		for (int spriteY = 0; spriteY <= maxY - minY; ++spriteY)
			if (x + spriteX >= 0 && x + spriteX < screenWidth && y + spriteY >= 0 && spriteY < screenHeight)
				if (sprite.GetPixel(spriteX, spriteY) != 0)
					DrawChar(x + spriteX, y + spriteY, sprite.GetPixel(minX + spriteX, minY + spriteY), sprite.GetColour(minX + spriteX, minY + spriteY));
}

/**
 * GetGreyScaleColour()
 * Given a luminosity between 0.0f and 1.0f, will return a grey scaled colour to that value.
 * 0.0f will give a black and 1.0f will give a white.
 * @param lum The brightness of the colour.
 * @return Character information about the character and colour used to create the desired colour.
 */
CHAR_INFO Engine::GameEngine::GetGreyScaleColour(const float& lum)
{
	short bgCol, fgCol;
	short sym;
	int pixelBw = (int)(13.0f * lum);

	switch (pixelBw)
	{
	case 0: bgCol = BG_BLACK; fgCol = FG_DARK_GREY; sym = PIXEL_QUARTER; break;

	case 1: bgCol = BG_BLACK; fgCol = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
	case 2: bgCol = BG_BLACK; fgCol = FG_DARK_GREY; sym = PIXEL_HALF; break;
	case 3: bgCol = BG_BLACK; fgCol = FG_DARK_GREY; sym = PIXEL_THREEQUARTER; break;
	case 4: bgCol = BG_BLACK; fgCol = FG_DARK_GREY; sym = PIXEL_SOLID; break;

	case 5: bgCol = BG_DARK_GREY; fgCol = FG_GREY; sym = PIXEL_QUARTER; break;
	case 6: bgCol = BG_DARK_GREY; fgCol = FG_GREY; sym = PIXEL_HALF; break;
	case 7: bgCol = BG_DARK_GREY; fgCol = FG_GREY; sym = PIXEL_THREEQUARTER; break;
	case 8: bgCol = BG_DARK_GREY; fgCol = FG_GREY; sym = PIXEL_SOLID; break;

	case 9:  bgCol = BG_GREY; fgCol = FG_WHITE; sym = PIXEL_QUARTER; break;
	case 10: bgCol = BG_GREY; fgCol = FG_WHITE; sym = PIXEL_HALF; break;
	case 11: bgCol = BG_GREY; fgCol = FG_WHITE; sym = PIXEL_THREEQUARTER; break;
	case 12: bgCol = BG_GREY; fgCol = FG_WHITE; sym = PIXEL_SOLID; break;
	default: bgCol = BG_BLACK; fgCol = FG_BLACK; sym = PIXEL_SOLID;
	}

	CHAR_INFO character;
	character.Char.UnicodeChar = sym;
	character.Attributes = bgCol | fgCol;
	return character;
}

/**
 * GetGreyScaleColour()
 * Given a luminosity between 0.0f and 1.0f, will return a colour with that brightness.
 * 0.0f will give a black and 1.0f will give a white.
 * @param baseColour The colour to be used as the base colour
 * @param lum The brightness of the colour.
 * @return Character information about the character and colour used to create the desired colour.
 */
CHAR_INFO Engine::GameEngine::GetColour(const short& baseColour, const float& lum)
{
	short darkColour = baseColour - 8;

	short bgCol, fgCol;
	short sym;
	int pixelBw = (int)(19.0f * lum);

	switch (pixelBw)
	{
	case 0: bgCol = BG_BLACK; fgCol = BG_BLACK; sym = PIXEL_SOLID; break;

	case 1: bgCol = BG_BLACK; fgCol = darkColour; sym = PIXEL_QUARTER; break;
	case 2: bgCol = BG_BLACK; fgCol = darkColour; sym = PIXEL_HALF; break;

	case 3: bgCol = BG_BLACK; fgCol = baseColour; sym = PIXEL_QUARTER; break;
	case 4: bgCol = BG_BLACK; fgCol = baseColour; sym = PIXEL_HALF; break;

	case 5: bgCol = BG_BLACK; fgCol = darkColour; sym = PIXEL_THREEQUARTER; break;
	case 6: bgCol = BG_BLACK; fgCol = darkColour; sym = PIXEL_SOLID; break;
	case 7: bgCol = BG_DARK_GREY; fgCol = darkColour; sym = PIXEL_SOLID; break;
	case 8: bgCol = BG_GREY; fgCol = darkColour; sym = PIXEL_SOLID; break;
	case 9: bgCol = BG_WHITE; fgCol = darkColour; sym = PIXEL_SOLID; break;

	case 10: bgCol = BG_BLACK; fgCol = baseColour; sym = PIXEL_THREEQUARTER; break;
	case 11: bgCol = BG_BLACK; fgCol = baseColour; sym = PIXEL_SOLID; break;
	case 12: bgCol = BG_DARK_GREY; fgCol = baseColour; sym = PIXEL_SOLID; break;
	case 13: bgCol = BG_GREY; fgCol = baseColour; sym = PIXEL_SOLID; break;
	case 14: bgCol = BG_WHITE; fgCol = baseColour; sym = PIXEL_SOLID; break;
	case 15: bgCol = BG_WHITE; fgCol = baseColour; sym = PIXEL_THREEQUARTER; break;
	case 16: bgCol = BG_WHITE; fgCol = baseColour; sym = PIXEL_HALF; break;
	case 17: bgCol = BG_WHITE; fgCol = baseColour; sym = PIXEL_QUARTER; break;

	case 18: bgCol = BG_WHITE; fgCol = FG_WHITE; sym = PIXEL_QUARTER; break;

	default: bgCol = BG_BLACK; fgCol = FG_BLACK; sym = PIXEL_SOLID;
	}

	CHAR_INFO character;
	character.Char.UnicodeChar = sym;
	character.Attributes = bgCol | fgCol;
	return character;
}