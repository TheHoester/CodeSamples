#pragma once
#include <Windows.h>

#include "FVector2.h"
#include "Singleton.h"

using namespace Engine::Physics;

enum ButtonState
{
	Up = 0, Released = 1, Pressed = 2, Held = 3
};

/**
	* InputHandler
	* Keeps track of the entire keyboard state at runtime and provides easy access.
	*/
class InputHandler : public Engine::Singleton<InputHandler>
{
	friend class Engine::Singleton<InputHandler>;

private:
	HANDLE consoleIn;

	ButtonState keyboardState[256];
	short previousKeyState[256];
	ButtonState mouseButtonState[5];
	bool currentMouseState[5];
	bool previousMouseState[5];
	FVector2 mousePosition;

	InputHandler(void);
public:
	~InputHandler(void);

	void UpdateKeyState(void);

	bool IsKeyPressed(const unsigned short& key) const;
	bool IsKeyReleased(const unsigned short& key) const;
	bool IsKeyHeld(const unsigned short& key) const;
	FVector2 GetMousePosition(void) const;
	bool IsMouseButtonPressed(const unsigned short& button) const;
	bool IsMouseButtonReleased(const unsigned short& button) const;
	bool IsMouseButtonHeld(const unsigned short& button) const;
};