#include "FirstPerson.h"

/*
 * Constructor (Default Map)
 * @param screenBuffer A pointer to the screenbuffer to be able to draw to.
 * @param input Pointer to the input handler.
 * @param time Pointer to the time handler.
 * @param appID The id of the application.
 * @param width Character width of the screen.
 * @param height Character height of the screen.
 * @param fontWidth Pixel width of the font.
 * @param fontHeight Pixel height of the font.
 */
FirstPerson::FirstPerson(CHAR_INFO* screenBuffer, InputHandler* input, Time* time, int appID, int width, int height, int fontWidth, int fontHeight) :
	Application(screenBuffer, input, time, appID, width, height, fontWidth, fontHeight), mapWidth(32), mapHeight(32), player(2.0f, 2.0f),
	direction(sinf(playerA), cosf(playerA)), moveVelocity(0.0f, 0.0f), playerA(0.0f)
{
	GenerateAssets();
}

/*
 * Constructor (Custom Map)
 * @param screenBuffer A pointer to the screenbuffer to be able to draw to.
 * @param input Pointer to the input handler.
 * @param time Pointer to the time handler.
 * @param appID The id of the application.
 * @param width Character width of the screen.
 * @param height Character height of the screen.
 * @param fontWidth Pixel width of the font.
 * @param fontHeight Pixel height of the font.
 * @param mapWidth Character width of the map.
 * @param mapHeight Character height of the map.
 * @param map The map to be created.
 */
FirstPerson::FirstPerson(CHAR_INFO* screenBuffer, InputHandler* input, Time* time, int appID, int width, int height, int fontWidth, int fontHeight, int mapWidth, int mapHeight, std::wstring map) :
	Application(screenBuffer, input, time, appID, width, height, fontWidth, fontHeight), mapWidth(mapWidth), mapHeight(mapHeight), map(map), player(2.0f, 2.0f),
	direction(sinf(playerA), cosf(playerA)), moveVelocity(0.0f, 0.0f), playerA(0.0f) { }

/**
 * Destructor
 */
FirstPerson::~FirstPerson() { }

/*
 * Update()
 * Public call to the main update function for the game.
 * @return 0 is the game should exit to menu, else the game ID number.
 */
int FirstPerson::Update()
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
void FirstPerson::GameLogic()
{
	// 'W' = Move Forwards
	if (input->IsKeyHeld('W'))
	{
		moveVelocity = direction * moveSpeed * time->GetDeltaTime();
		player += moveVelocity;

		if (map[((int)player.y * mapWidth) + (int)player.x] == '#')
			player -= moveVelocity;
	}
	
	// 'S' = Move Backwards
	if (input->IsKeyHeld('S'))
	{
		moveVelocity = direction * moveSpeed * time->GetDeltaTime();
		player -= moveVelocity;

		if (map[((int)player.y * mapWidth) + (int)player.x] == '#')
			player += moveVelocity;
	}

	// 'A' = Turn Left
	if (input->IsKeyHeld('A'))
		playerA -= (rotationSpeed * time->GetDeltaTime());

	// 'D' = Turn Right
	if (input->IsKeyHeld('D'))
		playerA += (rotationSpeed * time->GetDeltaTime());

	direction.x = sinf(playerA);
	direction.y = cosf(playerA);

	// 'Q' = Move Left
	if (input->IsKeyHeld('Q'))
	{
		moveVelocity = FVector2(-cosf(playerA), sinf(playerA)) * moveSpeed * time->GetDeltaTime();
		player += moveVelocity;

		if (map[((int)player.y * mapWidth) + (int)player.x] == '#')
			player -= moveVelocity;
	}

	// 'E' = Move Right
	if (input->IsKeyHeld('E'))
	{
		moveVelocity = FVector2(cosf(playerA), -sinf(playerA)) * moveSpeed * time->GetDeltaTime();
		player += moveVelocity;

		if (map[((int)player.y * mapWidth) + (int)player.x] == '#')
			player -= moveVelocity;
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

		float distanceToWall = 0.0f;
		bool hitWall = false;
		bool hitBoundry = false;

		float eyeX = sinf(rayAngle); // Unit Vector for ray in player space
		float eyeY = cosf(rayAngle);

		while (!hitWall && distanceToWall < depthOfField)
		{
			distanceToWall += 0.1f;

			// Position of ray hit on the map
			int testX = (int)(player.x + (eyeX * distanceToWall));
			int testY = (int)(player.y + (eyeY * distanceToWall));

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

					std::vector<std::pair<float, float>> points; // distance, dot product
					for (int pX = 0; pX < 2; ++pX)
					{
						for (int pY = 0; pY < 2; ++pY)
						{
							float vY = (float)testY + pY - player.y;
							float vX = (float)testX + pX - player.x;
							float dist = sqrt((vX * vX) + (vY *vY));
							float dot = ((eyeX * vX) / dist) + ((eyeY * vY) / dist);
							points.push_back(std::make_pair(dist, dot));
						}
					}

					// Sort Pairs from closest to farthest
					sort(points.begin(), points.end(), [](const std::pair<float, float> &left, const std::pair<float, float> &right) { return left.first < right.first; });

					float bound = 0.01f;
					if (acos(points.at(0).second) < bound)
						hitBoundry = true;
					if (acos(points.at(1).second) < bound)
						hitBoundry = true;
				}
			}
		}

		// Calculate distance to ceiling and floor
		int ceiling = (float)(screenHeight / 2.0) - (screenHeight / ((float)distanceToWall));
		int floor = screenHeight - ceiling;

		for (int y = 0; y < screenHeight; ++y)
		{
			if (y < ceiling)
				GameEngine::DrawChar(screenBuffer, screenWidth, screenHeight, x, y, ' ');
			else if (y > ceiling && y <= floor)
			{
				// Shade wall based on distance
				short wallShade = ' ';
				if (hitBoundry)
					wallShade = ' ';
				else if (distanceToWall <= depthOfField / 4.0f) // Very close
					wallShade = PIXEL_SOLID;
				else if (distanceToWall <= depthOfField / 3.0f)
					wallShade = PIXEL_THREEQUARTER;
				else if (distanceToWall <= depthOfField / 2.0f)
					wallShade = PIXEL_HALF;
				else if (distanceToWall <= depthOfField)
					wallShade = PIXEL_QUARTER;

				GameEngine::DrawChar(screenBuffer, screenWidth, screenHeight, x, y, wallShade);
			}
			else
			{
				// Shade floor based on distance
				short floorShade = ' ';
				float distance = 1.0f - (((float)y - (screenHeight / 2.0f)) / ((float)screenHeight / 2.0f));
				if (distance < 0.25f)
					floorShade = '#';
				else if (distance < 0.5f)
					floorShade = 'x';
				else if (distance < 0.75f)
					floorShade = '-';
				else if (distance < 0.9f)
					floorShade = '.';

				GameEngine::DrawChar(screenBuffer, screenWidth, screenHeight, x, y, floorShade);
			}
		}
	}

	// Display Map
	for (int x = 0; x < mapWidth; ++x)
		for (int y = 0; y < mapHeight; ++y)
			GameEngine::DrawChar(screenBuffer, screenWidth, screenHeight, x, y, map[(y * mapWidth) + x]);

	GameEngine::DrawChar(screenBuffer, screenWidth, screenHeight, (int)player.x, (int)player.y + 1, 'P');
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

	GameEngine::ClearScreen(screenBuffer, screenWidth, screenHeight);
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
}