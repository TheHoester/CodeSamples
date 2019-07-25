#include "AutoMaze.h"

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
 * @param mazeWidth The cell width of the maze.
 * @param mazeHeight The cell height of the maze.
 */
AutoMaze::AutoMaze(GameEngine* engine, int appID, int width, int height, int fontWidth, int fontHeight, int mazeWidth, int mazeHeight) : Application(engine, appID, width, height, fontWidth, fontHeight),
	mazeWidth(mazeWidth), mazeHeight(mazeHeight), visitedCount(0), startPosition(0, 0)
{
	visited = new bool[mazeWidth * mazeHeight];
	memset(visited, 0, sizeof(bool) * mazeWidth * mazeHeight);

	GenerateAssets();
}

/*
 * Destructor
 */
AutoMaze::~AutoMaze()
{
	if (visited != NULL)
		delete[] visited;
}

/*
 * Update()
 * Public call to the main update function for the game.
 * @return 0 is the game should exit to menu, else the game ID number.
 */
int AutoMaze::Update()
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
void AutoMaze::GameLogic()
{
	if (visitedCount < mazeWidth * mazeHeight)
	{
		// Get random neighbour
		Vector2 currentNeighbour = GetRandomNeighbour(path.top());

		// If no neighbours - pop off path stack
		if (currentNeighbour.x == -1)
			path.pop();
		else // Else add neighbour to stack
		{
			// Update visited array and count
			SetVisited(currentNeighbour);
			++visitedCount;
			
			// Draw where the wall use to be
			if (currentNeighbour.y < path.top().y) // Up
				for (int i = 0; i < pathWidth; i++)
					engine->DrawChar((path.top().x * (pathWidth + 1)) + i, (path.top().y * (pathWidth + 1)) - 1 , PIXEL_SOLID, FG_WHITE);
			else if (currentNeighbour.x > path.top().x) // Right
				for (int i = 0; i < pathWidth; i++)
					engine->DrawChar((path.top().x * (pathWidth + 1)) + pathWidth, (path.top().y * (pathWidth + 1)) + i, PIXEL_SOLID, FG_WHITE);
			else if (currentNeighbour.y > path.top().y) // Down
				for (int i = 0; i < pathWidth; i++)
					engine->DrawChar((path.top().x * (pathWidth + 1)) + i, (path.top().y * (pathWidth + 1)) + pathWidth, PIXEL_SOLID, FG_WHITE);
			else if (currentNeighbour.x < path.top().x) // Left
				for (int i = 0; i < pathWidth; i++)
					engine->DrawChar((path.top().x * (pathWidth + 1)) - 1, (path.top().y * (pathWidth + 1)) + i, PIXEL_SOLID, FG_WHITE);

			path.push(currentNeighbour);
		}	
	}
}

/*
 * Draw()
 * Draws the game to the screen buffer.
 */
void AutoMaze::Draw()
{
	for (int x = 0; x < mazeWidth; ++x)
	{
		for (int y = 0; y < mazeHeight; ++y)
		{
			if (GetVisited(x, y))
				engine->DrawRectFill(x * (pathWidth + 1), y * (pathWidth + 1), (x * (pathWidth + 1)) + pathWidth - 1, (y * (pathWidth + 1)) + pathWidth - 1, PIXEL_SOLID, FG_WHITE, PIXEL_SOLID, FG_WHITE);
			else
				engine->DrawRectFill(x * (pathWidth + 1), y * (pathWidth + 1), (x * (pathWidth + 1)) + pathWidth - 1, (y * (pathWidth + 1)) + pathWidth - 1, PIXEL_SOLID, FG_BLUE, PIXEL_SOLID, FG_BLUE);
		}
	}

	engine->DrawRectFill(
		 path.top().x * (pathWidth + 1),                   path.top().y * (pathWidth + 1), 
		(path.top().x * (pathWidth + 1)) + pathWidth - 1, (path.top().y * (pathWidth + 1)) + pathWidth - 1, 
		PIXEL_SOLID, FG_GREEN, PIXEL_SOLID, FG_GREEN);
}

/*
 * Reset()
 * Resets the game back to the beginning state.
 */
void AutoMaze::Reset()
{
	if (visited != NULL)
		delete[] visited;
	visited = new bool[mazeWidth * mazeHeight];
	memset(visited, 0, sizeof(bool) * mazeWidth * mazeHeight);

	while (!path.empty())
		path.pop();

	visitedCount = 0;

	GenerateAssets();
	engine->ClearScreen();
}

/*
 * GenerateAssets()
 * Initilizes all the assets for the game.
 */
void AutoMaze::GenerateAssets()
{
	path.push(startPosition);
	SetVisited(startPosition);
	++visitedCount;
}
/*
 * GetVisited()
 * Returns whether a cell has been visited at the given coordinates.
 * If the coords are invalid it will return true to simluate not being able to move here.
 * @param x The X coordinate of the cell.
 * @param y The Y coordinate of the cell.
 * @return True if the cell has been visited or is invalid, else false.
 */
bool AutoMaze::GetVisited(const int& x, const int& y) const
{
	if (x >= 0 && x < mazeWidth && y >= 0 && y < mazeHeight)
		return visited[(y * mazeWidth) + x];
	else
		return true;
}

/*
 * GetRandomNeightbor
 * Returns the position of a random unvisited neighbour.
 * If there are no unvisited neighbours then a vector of -1,-1 is returned.
 * @param x The X coordinate of the cell.
 * @param y The Y coordinate of the cell.
 * @return Position of the neighbour cell selected.
 */
Vector2 AutoMaze::GetRandomNeighbour(const int& x, const int& y) const
{
	std::vector<Vector2> options;

	if (!GetVisited(x, y - 1)) options.push_back(Vector2(x, y - 1)); // Up
	if (!GetVisited(x + 1, y)) options.push_back(Vector2(x + 1, y)); // Right
	if (!GetVisited(x, y + 1)) options.push_back(Vector2(x, y + 1)); // Down
	if (!GetVisited(x - 1, y)) options.push_back(Vector2(x - 1, y)); // Left

	if (options.empty())
		return Vector2(-1, -1);
	else
		return options[rand() % options.size()];
}
Vector2 AutoMaze::GetRandomNeighbour(const Vector2& pos) const { return GetRandomNeighbour(pos.x, pos.y); }

/*
 * SetVisited()
 * Sets the cell as the given coordinates as visited.
 * @param x The X coordinate of the cell.
 * @param y The Y coordinate of the cell.
 */
void AutoMaze::SetVisited(const int& x, const int& y)
{
	if (x >= 0 && x < mazeWidth && y >= 0 && y < mazeHeight)
		visited[(y * mazeWidth) + x] = true;
}
void AutoMaze::SetVisited(const Vector2& pos) { SetVisited(pos.x, pos.y); }
