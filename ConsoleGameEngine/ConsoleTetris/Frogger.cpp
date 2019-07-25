#include "Frogger.h"


Frogger::Frogger(GameEngine* engine, int appID, int width, int height, int fontWidth, int fontHeight) : Application(engine, appID, width, height, fontWidth, fontHeight)
{
	GenerateAssets();
}

Frogger::~Frogger(void)
{
	if (bus != nullptr)
		delete bus;
	if (car != nullptr)
		delete car;
	if (logLeft != nullptr)
		delete logLeft;
	if (logMiddle != nullptr)
		delete logMiddle;
	if (logRight != nullptr)
		delete logRight;
	if (path != nullptr)
		delete path;
	if (wall != nullptr)
		delete wall;
	if (water != nullptr)
		delete water;
	if (frog != nullptr)
		delete frog;

	if (dangerBuffer != nullptr)
		delete[] dangerBuffer;
}

/*
 * Update()
 * Public call to the main update function for the game.
 * @return 0 is the game should exit to menu, else the game ID number.
 */
int Frogger::Update()
{
	if (!player->isActive)
		player->isActive = true;

	GameLogic();
	Draw();

	if (InputHandler::Instance().IsKeyPressed(VK_RETURN))
		Reset();
	if (InputHandler::Instance().IsKeyPressed(VK_ESCAPE))
	{
		player->isActive = false;
		Reset();
		return 0;
	}

	return appID;
}

void Frogger::GameLogic(void)
{
	// Player Input
	if (InputHandler::Instance().IsKeyPressed(VK_LEFT))
		--player->worldPosition.x;
	if (InputHandler::Instance().IsKeyPressed(VK_RIGHT))
		++player->worldPosition.x;
	if (InputHandler::Instance().IsKeyPressed(VK_UP))
		--player->worldPosition.y;
	if (InputHandler::Instance().IsKeyPressed(VK_DOWN))
		++player->worldPosition.y;

	// Move Player on Log
	if (player->worldPosition.y <= 3)
		player->worldPosition.x -= lanes[(int)player->worldPosition.y].first * Time::Instance().DeltaTime();

	// Edge Detection
	if (player->worldPosition.x < 0)
		player->worldPosition.x = 0;
	if (player->worldPosition.x >= (screenWidth / cellSize))
		player->worldPosition.x = (screenWidth / cellSize) - 1;
	if (player->worldPosition.y < 0)
		player->worldPosition.y = 0;
	if (player->worldPosition.y >= (screenHeight / cellSize))
		player->worldPosition.y = (screenHeight / cellSize) - 1;

	// Check Danger Buffer For Collision
	bool topLeft =     dangerBuffer[((int)((player->worldPosition.y * cellSize) + 1)       * screenWidth) + (int)((player->worldPosition.x       * cellSize) + 1)];
	bool topRight =    dangerBuffer[((int)((player->worldPosition.y * cellSize) + 1)       * screenWidth) + (int)(((player->worldPosition.x + 1) * cellSize) - 1)];
	bool bottomLeft =  dangerBuffer[((int)(((player->worldPosition.y + 1) * cellSize) - 1) * screenWidth) + (int)((player->worldPosition.x       * cellSize) + 1)];
	bool bottomRight = dangerBuffer[((int)(((player->worldPosition.y + 1) * cellSize) - 1) * screenWidth) + (int)(((player->worldPosition.x + 1) * cellSize) - 1)];

	// Dead - Reset player
	if (topLeft || topRight || bottomLeft || bottomRight)
		player->worldPosition = FVector2(8.0f, 9.0f);

	player->screenPosition = player->worldPosition * cellSize;
}

void Frogger::Draw(void)
{
	engine->ClearScreen();

	// Draw Lanes
	int x = -1, y = 0;
	for (auto lane : lanes)
	{
		// Lane offset
		int startPos = (int)(Time::Instance().TimeSinceStart() * lane.first) % laneLength;
		if (startPos < 0) 
			startPos = laneLength - (abs(startPos) % laneLength);

		int cellOffset = (int)((float)cellSize * Time::Instance().TimeSinceStart() * lane.first) % cellSize;

		// Flip Car Sprite
		if (lane.first < 0)
			car->SetScale(-1.0f, 1.0f);
		else
			car->SetScale(1.0f, 1.0f);

		for (int i = 0; i < (screenWidth / cellSize) + 2; ++i)
		{
			wchar_t graphic = lane.second[(startPos + i) % laneLength];

			switch (graphic)
			{
			case 'b':
				engine->DrawSprite(((x + i) * cellSize) - cellOffset, y * cellSize, *bus);
				break;
			case 'c':
				engine->DrawSprite(((x + i) * cellSize) - cellOffset, y * cellSize, *car);
				break;
			case 'l':
				engine->DrawSprite(((x + i) * cellSize) - cellOffset, y * cellSize, *logLeft);
				break;
			case 'm':
				engine->DrawSprite(((x + i) * cellSize) - cellOffset, y * cellSize, *logMiddle);
				break;
			case 'r':
				engine->DrawSprite(((x + i) * cellSize) - cellOffset, y * cellSize, *logRight);
				break;
			case 'n':
				engine->DrawSprite(((x + i) * cellSize) - cellOffset, y * cellSize, *wall);
				break;
			case 'w':
				engine->DrawSprite(((x + i) * cellSize) - cellOffset, y * cellSize, *water);
				break;
			case 'p': case 'z':
				engine->DrawSprite(((x + i) * cellSize) - cellOffset, y * cellSize, *path);
				break;
			}

			// Update Danger Buffer
			for (int j = ((x + i) * cellSize) - cellOffset; j < ((x + i + 1) * cellSize) - cellOffset; ++j)
				for (int k = y * cellSize; k < (y + 1) * cellSize; ++k)
					if (j >= 0 && j < screenWidth && k >= 0 && k < screenHeight)
						dangerBuffer[(k * screenWidth) + j] = (graphic == 'c' || graphic == 'b' || graphic == 'n' || graphic == 'w' || graphic == 'x');
		}

		++y;
	}
}

void Frogger::Reset(void)
{

}

void Frogger::GenerateAssets(void)
{
	bus = new Sprite(L"../Assets/Bus.spr");
	car = new Sprite(L"../Assets/Car.spr");
	logLeft = new Sprite(L"../Assets/LogLeft.spr");
	logMiddle = new Sprite(L"../Assets/LogMiddle.spr");
	logRight = new Sprite(L"../Assets/LogRight.spr");
	path = new Sprite(L"../Assets/Path.spr");
	wall = new Sprite(L"../Assets/Wall.spr");
	water = new Sprite(L"../Assets/Water.spr");
	frog = new Sprite(L"../Assets/Frog.spr");

	lanes = 
	{
		{ 0.0f, L"nnzznnnzznnnzznnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn" },
		{-3.0f, L"wwwwwwlmrwwwwwwlmmrwwwwwwwlrwwwwwwlrwwwwwlmrwwwwwwlmmrwwwwwlmrww" }, // Logs
		{ 3.0f, L"lmmrwwwwwwlrwwwwlrwwwwwlmrwwwwwwlrwwwwwlmmrwwwwwwlrwwwwwwwwwlmrw" }, // Logs
		{ 2.0f, L"wwwwwlrwwwwwlmrwwwwwwwlmmrwwwwwwlrwwwwwwlrwwwwwlmrwwwwwwlmmrwwww" }, // Logs
		{ 0.0f, L"pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp" }, // Path
		{ 3.0f, L"cx.....cx........cx....cx.......cx......cx.......cx......cx....." }, // Cars
		{-3.0f, L"....bxxx......bxxx.....bxxx.......bxxx.......bxxx......bxxx....." }, // Buses
		{ 4.0f, L".....cx....cx....cx.......cx.....cx.......cx...cx...cx.....cx..." }, // Cars
		{-2.0f, L".....cx...cx....cx...cx.....cx....cx....cx.......cx...cx....cx.." }, // Cars
		{ 0.0f, L"pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp" } // Path
	};

	dangerBuffer = new bool[screenWidth * screenHeight];
	memset(dangerBuffer, 0, sizeof(bool) * screenWidth * screenHeight);

	player = engine->CreateGameObject(8.0f, 9.0f, frog);
	player->screenPosition = player->worldPosition * cellSize;
	player->isActive = false;
}