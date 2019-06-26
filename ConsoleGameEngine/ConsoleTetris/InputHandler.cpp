#include "InputHandler.h"

/**
 * IsKeyDown()
 * Gets whether the key given is down.
 * @param key The key to get the state of.
 * @return True is the key is down, else false.
 */
bool InputHandler::IsKeyDown(const unsigned char key)
{
	return GetAsyncKeyState(key) != 0;
}

/**
 * IsKeyUp()
 * Gets whether the key given is up.
 * @param key The key to get the state of.
 * @return True is the key is up, else false.
 */
bool InputHandler::IsKeyUp(const unsigned char key)
{
	return GetAsyncKeyState(key) == 0;
}