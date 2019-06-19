#pragma once

#include <vector>
#include <windows.h>
#include "Defines.h"

class Map
{
private:
	static const int map[MAP_HEIGHT][MAP_WIDTH];

	static const int mapWidth;
	static const int mapHeight;

	static const int startX;
	static const int startY;

	static const int endX;
	static const int endY;

public:
	int memory[MAP_HEIGHT][MAP_WIDTH];

	Map(void) { ResetMemory(); }

	double TestRoute(const std::vector<int> &path, Map &memory);
	void Render(const int xClient, const int yClient, HDC surface);
	void ResetMemory(void);
};