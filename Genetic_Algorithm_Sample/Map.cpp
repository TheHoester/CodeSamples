#include "Map.h"

const int Map::map[MAP_HEIGHT][MAP_WIDTH] = { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, 
											   { 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1 }, 
											   { 8, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1 }, 
											   { 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1 }, 
											   { 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1 }, 
											   { 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 5 }, 
											   { 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1 }, 
											   { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };

const int Map::mapHeight = MAP_HEIGHT;
const int Map::mapWidth = MAP_WIDTH;
const int Map::startX = 15;
const int Map::startY = 5;
const int Map::endX = 0;
const int Map::endY = 2;

/**
 * Tests the path given against the map and returns a fitness
 * value of how far it is from the end goal.
 * @param path The path to be tested.
 * @param memory Reference to a map with memory of where the path has travelled.
 * @return The fitness value of the path.
 */
double Map::TestRoute(const std::vector<int> &path, Map &memory)
{
	int posX = startX;
	int posY = startY;

	for (int i = 0; i < path.size(); ++i)
	{
		switch (path[i])
		{
		case 0: // North (-y)
			if (posY - 1 >= 0 && map[posY - 1][posX] != 1)
				posY -= 1;
			break;
		case 1: // South (+y)
			if (posY + 1 < mapHeight && map[posY + 1][posX] != 1)
				posY += 1;
			break;
		case 2: // East (+x)
			if (posX + 1 < mapWidth && map[posY][posX + 1] != 1)
				posX += 1;
			break;
		case 3: // West (-x)
			if (posX - 1 >= 0 && map[posY][posX - 1] != 1)
				posX -= 1;
			break;
		}

		memory.memory[posY][posX] = 1;
	}

	int diffX = abs(posX - endX);
	int diffY = abs(posY - endY);

	return 1 / (double)(diffX + diffY + 1);
}

/**
 * Draws the maze in the window.
 * @param xClient Window height.
 * @param yClient Window width.
 * @param surface Handle to the backbuffer.
 */
void Map::Render(const int xClient, const int yClient, HDC surface)
{
	HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
	HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));

	for (int x = 0; x < mapWidth; ++x)
	{
		for (int y = 0; y < mapHeight; ++y)
		{
			switch (map[y][x])
			{
			case 1:
				SelectObject(surface, blackBrush);
				break;
			case 5:
				SelectObject(surface, redBrush);
				break;
			case 8:
				SelectObject(surface, greenBrush);
				break;
			}

			Rectangle(surface, x * (xClient / mapWidth), y * (yClient / mapHeight),
				x * (xClient / mapWidth) + (xClient / mapWidth), y * (yClient / mapHeight) + (yClient / mapHeight));

			SelectObject(surface, whiteBrush);
		}
	}

	DeleteObject(whiteBrush);
	DeleteObject(blackBrush);
	DeleteObject(redBrush);
	DeleteObject(greenBrush);
}

/**
 * Resets the memory of the map to  the default.
 */
void Map::ResetMemory()
{
	for (int x = 0; x < mapWidth; ++x)
		for (int y = 0; y < mapHeight; ++y)
			memory[y][x] = 0;
}