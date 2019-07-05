#pragma once
#include <Windows.h>

namespace Engine { namespace Input {
	enum KeyboardState
	{
		Up = 0, Released = 1, Pressed = 2, Held = 3
	};

	/**
	 * InputHandler
	 * Keeps track of the entire keyboard state at runtime and provides easy access.
	 */
	class InputHandler
	{
	private:
		KeyboardState keyboardState[256];
		short previousKeyState[256];

	public:
		InputHandler(void);
		~InputHandler(void);

		void UpdateKeyState(void);

		bool IsKeyPressed(const unsigned short& key) const;
		bool IsKeyReleased(const unsigned short& key) const;
		bool IsKeyHeld(const unsigned short& key) const;
	};
}
}