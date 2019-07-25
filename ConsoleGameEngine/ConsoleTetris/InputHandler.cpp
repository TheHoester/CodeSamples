#include "InputHandler.h"

/*
 * Constructor
 */
InputHandler::InputHandler() : mousePosition(0.0f, 0.0f)
{
	consoleIn = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(consoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

	memset(keyboardState, 0, sizeof(ButtonState) * 256);
	memset(previousKeyState, 0, sizeof(short) * 256);
	memset(mouseButtonState, 0, sizeof(ButtonState) * 5);
	memset(previousMouseState, 0, sizeof(bool) * 5);
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
	// Keyboard State
	for (int i = 0; i < 256; ++i)
	{
		short state = GetAsyncKeyState(i);
		if (state == 0 && previousKeyState[i] != 0)
			keyboardState[i] = ButtonState::Released;
		else if (state != 0 && previousKeyState[i] == 0)
			keyboardState[i] = ButtonState::Pressed;
		else if (state != 0 && previousKeyState[i] != 0)
			keyboardState[i] = ButtonState::Held;
		else
			keyboardState[i] = ButtonState::Up;

		previousKeyState[i] = state;
	}

	// Get Mouse Events
	INPUT_RECORD inBuf[32];
	DWORD events = 0;
	GetNumberOfConsoleInputEvents(consoleIn, &events);
	if (events > 0)
		ReadConsoleInput(consoleIn, inBuf, events, &events);

	// Read Mouse Events
	for (int i = 0; i < events; ++i)
	{
		switch (inBuf[i].EventType)
		{
		case MOUSE_EVENT:
			switch (inBuf[i].Event.MouseEvent.dwEventFlags)
			{
			case MOUSE_MOVED:
				mousePosition.x = inBuf[i].Event.MouseEvent.dwMousePosition.X;
				mousePosition.y = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
				break;
			case 0:
				for (int m = 0; m < 5; ++m)
					currentMouseState[m] = (inBuf[m].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
			}
			break;
		}
	}

	// Mouse State
	for (int m = 0; m < 5; ++m)
	{
		if (!currentMouseState[m] && previousMouseState[m])
			mouseButtonState[m] = ButtonState::Released;
		else if (currentMouseState[m] && !previousMouseState[m])
			mouseButtonState[m] = ButtonState::Pressed;
		else if (currentMouseState[m] && previousMouseState[m])
			mouseButtonState[m] = ButtonState::Held;
		else
			mouseButtonState[m] = ButtonState::Up;

		previousMouseState[m] = currentMouseState[m];
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

	return (keyboardState[key] == ButtonState::Pressed);
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

	return (keyboardState[key] == ButtonState::Released);
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

	return (keyboardState[key] == ButtonState::Held);
}

/**
 * GetMousePosition()
 * Gets the current position of the mouse on screen.
 * @return The current position of the mouse.
 */
FVector2 InputHandler::GetMousePosition() const { return mousePosition; }

/**
 * IsMouseButtonPressed()
 * Gets whether a mouse button has just been pressed.
 * @param button The mouse button to check.
 * @return True if the mouse button has been pressed (Previously up), else false.
 */
bool InputHandler::IsMouseButtonPressed(const unsigned short& button) const
{
	if (button >= 5)
		return false;
	if (mouseButtonState[button] == ButtonState::Pressed)
		return true;
	else 
		return false;
}

/**
 * IsMouseButtonReleased()
 * Gets whether a mouse button given has just been release.
 * @param button The mouse button to check.
 * @return True if the mouse button has been released (Previously down), else false.
 */
bool InputHandler::IsMouseButtonReleased(const unsigned short& button) const
{
	if (button >= 5)
		return false;

	return (mouseButtonState[button] == ButtonState::Released);
}

/**
 * IsMouseButtonHeld()
 * Gets whether a mouse button given is being held.
 * @param button The mouse button to check.
 * @return True if the mouse button is being held (Previously down), else false.
 */
bool InputHandler::IsMouseButtonHeld(const unsigned short& button) const
{
	if (button >= 5)
		return false;

	return (mouseButtonState[button] == ButtonState::Held);
}