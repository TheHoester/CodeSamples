#include "InputHandler.h"

using namespace Engine::Input;

/*
 * Constructor
 */
InputHandler::InputHandler()
{
	memset(keyboardState, 0, sizeof(KeyboardState) * 256);
	memset(previousKeyState, 0, sizeof(short) * 256);
}

/*
 * Destructor
 */
InputHandler::~InputHandler() { }

/*
 * UpdateKeyState()
 * Saves the current state of the keyboard, determining whether a key has just been pressed,
 * just been released, being held or is up.
 */
void InputHandler::UpdateKeyState()
{
	for (int i = 0; i < 256; ++i)
	{
		short state = GetAsyncKeyState(i);
		if (state == 0 && previousKeyState[i] != 0)
			keyboardState[i] = KeyboardState::Released;
		else if (state != 0 && previousKeyState[i] == 0)
			keyboardState[i] = KeyboardState::Pressed;
		else if (state != 0 && previousKeyState[i] != 0)
			keyboardState[i] = KeyboardState::Held;
		else
			keyboardState[i] = KeyboardState::Up;

		previousKeyState[i] = state;
	}
}

/**
 * IsKeyPressed()
 * Gets whether a key has just been pressed.
 * @param key The key to check.
 * @return True if the key has been pressed (Previously up), else false.
 */
bool InputHandler::IsKeyPressed(const unsigned short& key) const
{
	if (key >= 256)
		return false;

	return (keyboardState[key] == KeyboardState::Pressed);
}

/**
 * IsKeyReleased()
 * Gets whether a key given has just been release.
 * @param key The key to check.
 * @return True if the key has been released (Previously down), else false.
 */
bool InputHandler::IsKeyReleased(const unsigned short& key) const
{
	if (key >= 256)
		return false;

	return (keyboardState[key] == KeyboardState::Released);
}

/**
 * IsKeyHeld()
 * Gets whether a key given is being held.
 * @param key The key to check.
 * @return True if the key is being held (Previously down), else false.
 */
bool InputHandler::IsKeyHeld(const unsigned short& key) const
{
	if (key >= 256)
		return false;

	return (keyboardState[key] == KeyboardState::Held);
}