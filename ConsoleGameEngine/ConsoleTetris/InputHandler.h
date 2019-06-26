#pragma once
#include <Windows.h>

/**
 * InputHandler
 * Provides more readable and usable access to input handling.
 */
class InputHandler
{
private:

public:
	static bool IsKeyDown(const unsigned char key);
	static bool IsKeyUp(const unsigned char key);
};