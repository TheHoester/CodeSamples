#include "FirstPerson.h"

/**
 * Constructor
 * @param rend The render engine used to draw to the console.
 * @param time The time object that is used to track the length of time between frames.
 * @param screenWidth Character width of the screen.
 * @param screenHeight Character height of the screen.
 * @param fieldWidth Character width of the map.
 * @param fieldHeight Character height of the map.
 */
FirstPerson::FirstPerson(RenderEngine* rend, Time* time, int screenWidth, int screenHeight) : Application(rend, time, screenWidth, screenHeight), 
	player(8.0f, 8.0f), direction(sinf(playerA), cosf(playerA)), moveVelocity(0.0f, 0.0f), playerA(0.0f), moveSpeed(5.0f), rotationSpeed(1.5f)
{
	Application::fieldWidth = 16;
	Application::fieldHeight = 16;

	// Assets
	GenerateAssets();

	// Input
	keys = new bool[5];
}

/**
 * Constructor
 * @param rend The render engine used to draw to the console.
 * @param time The time object that is used to track the length of time between frames.
 * @param screenWidth Character width of the screen.
 * @param screenHeight Character height of the screen.
 * @param fieldWidth Character width of the map.
 * @param fieldHeight Character height of the map.
 * @param map A 2D depiction of what the map should look like.
 */
FirstPerson::FirstPerson(RenderEngine* rend, Time* time, int screenWidth, int screenHeight, int fieldWidth, int fieldHeight, wstring map) : 
	Application(rend, time, screenWidth, screenHeight), map(map), player(8.0f, 8.0f), direction(sinf(playerA), cosf(playerA)), 
	moveVelocity(0.0f, 0.0f), playerA(0.0f), moveSpeed(5.0f), rotationSpeed(1.5f)
{
	Application::fieldWidth = fieldWidth;
	Application::fieldHeight = fieldHeight;

	// Input
	keys = new bool[5];
}

/**
 * Destructor
 */
FirstPerson::~FirstPerson() { }

/**
 * Update()
 * Handles running all the logic for the game.
 * @return The current gameover state.
 */
bool FirstPerson::Update()
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
void FirstPerson::Reset()
{
	Application::Reset();
	
	player = FVector2(8.0f, 8.0f);
	direction = FVector2(sinf(playerA), cosf(playerA));
	moveVelocity = FVector2(0.0f, 0.0f);
	playerA = 0.0f;
}

/**
 * InputHandler()
 * Handles getting the current state of all the input.
 */
void FirstPerson::InputHandler()
{
	keys[0] = InputHandler::IsKeyDown('W');
	keys[1] = InputHandler::IsKeyDown('S');
	keys[2] = InputHandler::IsKeyDown('A');
	keys[3] = InputHandler::IsKeyDown('D');
	keys[4] = InputHandler::IsKeyDown(VK_BACK);
}

/**
 * GameLogic()
 * Handles the changing of state depending on the input given.
 */
void FirstPerson::GameLogic()
{
	moveVelocity = direction * moveSpeed * time->GetDeltaTime();

	// 'W' = Move Forwards
	if (keys[0]) 
	{
		player += moveVelocity;

		if (map[((int)player.y * fieldWidth) + (int)player.x] == '#')
			player -= moveVelocity;
	}
	
	// 'S' = Move Backwards
	if (keys[1])
	{
		player -= moveVelocity;

		if (map[((int)player.y * fieldWidth) + (int)player.x] == '#')
			player += moveVelocity;
	}

	// 'A' = Turn Left
	if (keys[2])
		playerA -= (rotationSpeed * time->GetDeltaTime());

	// 'D' = Turn Right
	if (keys[3])
		playerA += (rotationSpeed * time->GetDeltaTime());

	// 'Escape' = Exit game
	gameOver = keys[4];

	direction.x = sinf(playerA);
	direction.y = cosf(playerA);
}

/**
 * Draw()
 * Handles drawing the game to the console.
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
			if (testX < 0 || testX >= fieldWidth || testY < 0 || testY >= fieldHeight)
			{
				hitWall = true;					// Just set distance to maximum depth
				distanceToWall = depthOfField;
			}
			else
			{
				if (map[(testY * fieldWidth) + testX] == '#') // Ray is inbounds so test to see if ray cell is a wall block
				{
					hitWall = true;

					vector<pair<float, float>> points; // distance, dot product
					for (int pX = 0; pX < 2; ++pX)
					{
						for (int pY = 0; pY < 2; ++pY)
						{
							float vY = (float)testY + pY - player.y;
							float vX = (float)testX + pX - player.x;
							float dist = sqrt((vX * vX) + (vY *vY));
							float dot = ((eyeX * vX) / dist) + ((eyeY * vY) / dist);
							points.push_back(make_pair(dist, dot));
						}
					}

					// Sort Pairs from closest to farthest
					sort(points.begin(), points.end(), [](const pair<float, float> &left, const pair<float, float> &right) { return left.first < right.first; });

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
				screen[(y * screenWidth) + x] = ' ';
			else if (y > ceiling && y <= floor)
			{
				// Shade wall based on distance
				short wallShade = ' ';
				if (hitBoundry)
					wallShade = ' ';
				else if (distanceToWall <= depthOfField / 4.0f) // Very close
					wallShade = 0x2588;
				else if (distanceToWall <= depthOfField / 3.0f)
					wallShade = 0x2593;
				else if (distanceToWall <= depthOfField / 2.0f)
					wallShade = 0x2592;
				else if (distanceToWall <= depthOfField)
					wallShade = 0x2591;

				screen[(y * screenWidth) + x] = wallShade;
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

				screen[(y * screenWidth) + x] = floorShade;
			}
		}
	}
	// Display Stats
	swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f", player.x, player.y, playerA, 1.0f / time->GetDeltaTime());

	// Display Map
	for (int x = 0; x < fieldWidth; ++x)
		for (int y = 0; y < fieldHeight; ++y)
			screen[((y + 1) * screenWidth) + x] = map[(y * fieldWidth) + x];

	screen[(((int)player.y + 1) * screenWidth) + (int)player.x] = 'P';

	screen[(screenWidth * screenHeight) - 1] = '\0';
	renderer->Draw(screen);
}

/**
 * GenerateAssets()
 * Used to populate all variables with the assets needed to draw the game.
 * Usually called in the constructor and the reset function.
 */
void FirstPerson::GenerateAssets()
{
	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#...........#..#";
	map += L"#...........#..#";
	map += L"#...........#..#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#........#######";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";
}