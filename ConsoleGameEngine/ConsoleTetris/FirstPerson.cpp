#include "FirstPerson.h"

/*
 * Constructor (Default Map)
 * @param screenBuffer A pointer to the screenbuffer to be able to draw to.
 * @param input Pointer to the input handler.
 * @param appID The id of the application.
 * @param width Character width of the screen.
 * @param height Character height of the screen.
 * @param fontWidth Pixel width of the font.
 * @param fontHeight Pixel height of the font.
 */
FirstPerson::FirstPerson(GameEngine* engine, int appID, int width, int height, int fontWidth, int fontHeight) : Application(engine, appID, width, height, fontWidth, fontHeight),
	mapWidth(32), mapHeight(32), player(2.0f, 2.0f), direction(sinf(playerA), cosf(playerA)), moveVelocity(0.0f, 0.0f), playerA(0.0f) 
{
	GenerateAssets();
}

/*
 * Constructor (Custom Map)
 * @param screenBuffer A pointer to the screenbuffer to be able to draw to.
 * @param input Pointer to the input handler.
 * @param appID The id of the application.
 * @param width Character width of the screen.
 * @param height Character height of the screen.
 * @param fontWidth Pixel width of the font.
 * @param fontHeight Pixel height of the font.
 * @param mapWidth Character width of the map.
 * @param mapHeight Character height of the map.
 * @param map The map to be created.
 */
FirstPerson::FirstPerson(GameEngine* engine, int appID, int width, int height, int fontWidth, int fontHeight, int mapWidth, int mapHeight, std::wstring map) : Application(engine, appID, width, height, fontWidth, fontHeight), 
	mapWidth(mapWidth), mapHeight(mapHeight), map(map), player(2.0f, 2.0f), direction(sinf(playerA), cosf(playerA)), moveVelocity(0.0f, 0.0f), playerA(0.0f) { }

/**
 * Destructor
 */
FirstPerson::~FirstPerson() 
{ 
	if (wallSprite != nullptr)
		delete wallSprite;
	if (lampSprite != nullptr)
		delete lampSprite;
	if (depthBuffer != nullptr)
		delete[] depthBuffer;
}

/*
 * Update()
 * Public call to the main update function for the game.
 * @return 0 is the game should exit to menu, else the game ID number.
 */
int FirstPerson::Update()
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
void FirstPerson::GameLogic()
{
	// 'W' = Move Forwards
	if (InputHandler::Instance().IsKeyHeld('W'))
	{
		moveVelocity = direction * moveSpeed * Time::Instance().DeltaTime();
		player += moveVelocity;

		if (map[((int)player.y * mapWidth) + (int)player.x] == '#')
			player -= moveVelocity;
	}
	
	// 'S' = Move Backwards
	if (InputHandler::Instance().IsKeyHeld('S'))
	{
		moveVelocity = direction * moveSpeed * Time::Instance().DeltaTime();
		player -= moveVelocity;

		if (map[((int)player.y * mapWidth) + (int)player.x] == '#')
			player += moveVelocity;
	}

	// 'A' = Turn Left
	if (InputHandler::Instance().IsKeyHeld('A'))
		playerA -= (rotationSpeed * Time::Instance().DeltaTime());

	// 'D' = Turn Right
	if (InputHandler::Instance().IsKeyHeld('D'))
		playerA += (rotationSpeed * Time::Instance().DeltaTime());

	direction.x = sinf(playerA);
	direction.y = cosf(playerA);

	// 'Q' = Move Left
	if (InputHandler::Instance().IsKeyHeld('Q'))
	{
		moveVelocity = FVector2(-cosf(playerA), sinf(playerA)) * moveSpeed * Time::Instance().DeltaTime();
		player += moveVelocity;

		if (map[((int)player.y * mapWidth) + (int)player.x] == '#')
			player -= moveVelocity;
	}

	// 'E' = Move Right
	if (InputHandler::Instance().IsKeyHeld('E'))
	{
		moveVelocity = FVector2(cosf(playerA), -sinf(playerA)) * moveSpeed * Time::Instance().DeltaTime();
		player += moveVelocity;

		if (map[((int)player.y * mapWidth) + (int)player.x] == '#')
			player -= moveVelocity;
	}

	// Fire Projectile
	if (InputHandler::Instance().IsKeyPressed(VK_SPACE))
	{
		Object ball;
		ball.position = FVector2(player.x, player.y);
		float noise = (((float)rand() / (float)RAND_MAX) - 0.5f) * 0.1f;
		ball.velocity = FVector2(sinf(playerA + noise) * 8.0f, cosf(playerA + noise) * 8.0f);
		ball.sprite = fireBallSprite;
		ball.remove = false;
		objects.push_back(ball);
	}
}

/*
 * Draw()
 * Draws the game to the screen buffer.
 */
void FirstPerson::Draw()
{
	for (int x = 0; x < screenWidth; ++x)
	{
		// For each column, calculate the projected ray angle into world space
		float rayAngle = (playerA - (fov / 2.0f)) + (((float)x / (float)screenWidth) * fov);

		float stepSize = 0.05f;
		float distanceToWall = 0.0f;
		bool hitWall = false;
		bool hitBoundry = false;

		FVector2 eye = FVector2(sinf(rayAngle), cosf(rayAngle)); // Unit Vector for ray in player space
		float sampleX = 0.0f;

		while (!hitWall && distanceToWall < depthOfField)
		{
			distanceToWall += stepSize;

			// Position of ray hit on the map
			int testX = (int)(player.x + (eye.x * distanceToWall));
			int testY = (int)(player.y + (eye.y * distanceToWall));

			// Test if ray is out of bounds
			if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight)
			{
				hitWall = true;					// Just set distance to maximum depth
				distanceToWall = depthOfField;
			}
			else
			{
				if (map[(testY * mapWidth) + testX] == '#') // Ray is inbounds so test to see if ray cell is a wall block
				{
					hitWall = true;

					float cellMidX = (float)testX + 0.5f;
					float cellMidY = (float)testY + 0.5f;

					float testPointX = player.x + eye.x * distanceToWall;
					float testPointY = player.y + eye.y * distanceToWall;

					float testAngle = atan2f((testPointY - cellMidY), (testPointX - cellMidX));

					// Get X sample position.
					if (testAngle >= (-PI * 0.25f) && testAngle < (PI * 0.25f))
						sampleX = testPointY - (float)testY;
					if (testAngle >= (PI * 0.25f) && testAngle < (PI * 0.75f))
						sampleX = testPointX - (float)testX;
					if (testAngle < (-PI * 0.25f) && testAngle >= (-PI * 0.75f))
						sampleX = testPointX - (float)testX;
					if (testAngle >= (PI * 0.75f) || testAngle < (-PI * 0.75f))
						sampleX = testPointY - (float)testY;
				}
			}
		}

		// Calculate distance to ceiling and floor
		int ceiling = (float)(screenHeight / 2.0) - (screenHeight / ((float)distanceToWall));
		int floor = screenHeight - ceiling;

		// Update depth buffer
		depthBuffer[x] = distanceToWall;

		for (int y = 0; y < screenHeight; ++y)
		{
			if (y <= ceiling)
				engine->DrawChar(x, y, ' ');
			else if (y > ceiling && y <= floor)
			{
				if (distanceToWall < depthOfField)
				{
					float sampleY = ((float)y - (float)ceiling) / ((float)floor - (float)ceiling);
					engine->DrawChar(x, y, wallSprite->SamplePixel(sampleX, sampleY), wallSprite->SampleColour(sampleX, sampleY));
				}
				else
					engine->DrawChar(x, y, ' ');

			}
			else
				engine->DrawChar(x, y, PIXEL_SOLID, FG_DARK_GREEN);
		}
	}

	// Draw Objects
	for (auto &object : objects)
	{
		// Update Object Physics
		object.position += object.velocity * Time::Instance().DeltaTime();

		// Object Collision
		if (map[((int)object.position.x * mapWidth) + (int)object.position.y] == '#')
			object.remove = true;

		FVector2 vec = FVector2(object.position.x - player.x, object.position.y - player.y);
		float distanceFromPlayer = vec.Magnitude();

		float objectAngle = atan2f(direction.y, direction.x) - atan2f(vec.y, vec.x);
		if (objectAngle < -PI)
			objectAngle += 2.0f * PI;
		if (objectAngle > PI)
			objectAngle -= 2.0f * PI;

		bool isInPlayerFOV = fabs(objectAngle < fov / 2.0f);

		if (isInPlayerFOV && distanceFromPlayer >= 0.5f && distanceFromPlayer < depthOfField)
		{
			float objectCeiling = (float)(screenHeight / 2.0f) - screenHeight / (float)(distanceFromPlayer);
			float objectFloor = screenHeight - objectCeiling;
			float objectHeight = objectFloor - objectCeiling;
			float objectAspectRatio = (float)(object.sprite->Height()) / (float)(object.sprite->Width());
			float objectWidth = objectHeight / objectAspectRatio;
			float middleOfObject = (0.5f * (objectAngle / (fov / 2.0f)) + 0.5f) * (float)screenWidth;

			for (float x = 0; x < objectWidth; ++x)
			{
				for (float y = 0; y < objectHeight; ++y)
				{
					FVector2 sample = FVector2(x / objectWidth, y / objectHeight);

					short character = object.sprite->SamplePixel(sample.x, sample.y);
					int objectColumn = (int)(middleOfObject + x - (objectWidth / 2.0f));
					if (objectColumn >= 0 && objectColumn < screenWidth && character != 0 && depthBuffer[objectColumn] >= distanceFromPlayer)
					{
						engine->DrawChar(objectColumn, objectCeiling + y, 
							character, object.sprite->SampleColour(sample.x, sample.y));
						depthBuffer[objectColumn] = distanceFromPlayer;
					}
				}
			}
		}
	}

	// Remove Dead Objects
	objects.remove_if([](Object &o) { return o.remove; });

	// Display Map
	for (int x = 0; x < mapWidth; ++x)
		for (int y = 0; y < mapHeight; ++y)
			engine->DrawChar(x, y, map[(y * mapWidth) + x]);

	engine->DrawChar((int)player.x, (int)player.y + 1, 'P');
}

/*
 * Reset()
 * Resets the game back to the beginning state.
 */
void FirstPerson::Reset()
{
	player = FVector2(2.0f, 2.0f);
	direction = FVector2(sinf(playerA), cosf(playerA));
	moveVelocity = FVector2(0.0f, 0.0f);
	playerA = 0.0f;

	engine->ClearScreen();
}

/*
 * GenerateAssets()
 * Initilizes all the assets for the game.
 */
void FirstPerson::GenerateAssets()
{
	map += L"################################";
	map += L"#...............#..............#";
	map += L"#.......#########.......########";
	map += L"#..............##..............#";
	map += L"#......##......##......##......#";
	map += L"#......##..............##......#";
	map += L"#..............##..............#";
	map += L"###............####............#";
	map += L"##.............###.............#";
	map += L"#............####............###";
	map += L"#..............................#";
	map += L"#..............##..............#";
	map += L"#..............##..............#";
	map += L"#...........#####...........####";
	map += L"#..............................#";
	map += L"###..####....########....#######";
	map += L"####.####.......######.........#";
	map += L"#...............#..............#";
	map += L"#.......#########.......##..####";
	map += L"#..............##..............#";
	map += L"#......##......##.......#......#";
	map += L"#......##......##......##......#";
	map += L"#..............##..............#";
	map += L"###............####............#";
	map += L"##.............###.............#";
	map += L"#............####............###";
	map += L"#..............................#";
	map += L"#..............................#";
	map += L"#..............##..............#";
	map += L"#............##.............####";
	map += L"#..............##..............#";
	map += L"################################";

	wallSprite = new Sprite(L"../Assets/BrickWall.spr");
	lampSprite = new Sprite(L"../Assets/Lamp.spr");
	fireBallSprite = new Sprite(L"../Assets/FireBall.spr");

	objects = {
		{ FVector2(8.5f, 8.5f), FVector2(0.0f, 0.0f), false, lampSprite },
		{ FVector2(7.5f, 7.5f), FVector2(0.0f, 0.0f), false, lampSprite },
		{ FVector2(3.5f, 10.5f), FVector2(0.0f, 0.0f), false, lampSprite }
	};

	depthBuffer = new float[screenWidth];
}