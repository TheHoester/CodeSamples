#pragma once
#include <iostream>
#include <Windows.h>

using namespace std;

/**
 * RenderEngine
 * Handles drawing to the console.
 */
class RenderEngine
{
private:
	HANDLE console;
	DWORD bytesWritten;

	int screenWidth;
	int screenHeight;

public:
	RenderEngine(int screenWidth, int screenHeight);
	~RenderEngine(void);

	void Draw(const wchar_t *characterArray);
};