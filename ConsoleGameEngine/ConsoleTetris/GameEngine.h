#pragma once
#include <thread>

#include "Colour.h"
#include "FVector2.h"
#include "InputHandler.h"
#include "RenderEngine.h"
#include "Time.h"

using namespace Engine::Graphics;
using namespace Engine::Input;
using namespace Engine::Physics;

/*
 * Vector2
 * Stores an X and Y coordinate as integers to avoid the inaccuracy of floats.
 */
struct Vector2
{
	int x;
	int y;

	Vector2(void) : x(0), y(0) { }
	Vector2(int x, int y) : x(x), y(y) { }

	Vector2& operator=(const Vector2& other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}

	bool operator==(const Vector2& other) const { return (x == other.x && y == other.y); }
};

namespace Engine
{
	/*
	 * GameEngine
	 * An abstract class that defines a simple to implement framework for any game.
	 * Classes implemented from this class must override:
	 *		- CreateGame() to initialize the game.
	 *		- RunGame() for all the logic and drawing (This function is used in the main game loop).
	 */
	class GameEngine
	{
	private:
		// Engine Functions
		void ThreadUpdate(void);

	protected:
		std::wstring appName;
		int screenWidth;
		int screenHeight;
		CHAR_INFO* screenBuffer;
		RenderEngine* renderer;
		InputHandler* inputHandler;
		Time* time;
		bool close;

		// Virtual Game Functions
		virtual bool CreateGame(void) = 0;
		virtual bool RunGame(void) = 0;

		// Draw Functions
		void ClearScreen();
		void DrawChar(const int& x, const int& y, const short& character = PIXEL_SOLID, const short& colour = FG_WHITE);
		void DrawRect(const int& minX, const int& minY, const int& maxX, const int& maxY, const short& character = PIXEL_SOLID, const short& colour = FG_WHITE);
		void DrawRectFill(const int& minX, const int& minY, const int& maxX, const int& maxY, const short& borderCharacter = PIXEL_SOLID, const short& borderColour = FG_WHITE, const short& fillCharacter = ' ', const short& fillColour = FG_BLACK);
		void DrawString(const int& x, const int& y, const std::wstring& msg, const short& colour = FG_WHITE);
		void DrawCircle(const int& centreX, const int& centreY, const int& radius, const short& character = PIXEL_SOLID, const short& colour = FG_WHITE);

	public:
		GameEngine(std::wstring name = L"GameEngine", int width = 80, int height = 30, int fontWidth = 8, int fontHeight = 16);
		~GameEngine(void);

		// Startup game - generate all assets needed
		void Start(void);

		// Draw Functions
		static void ClearScreen(CHAR_INFO* buffer, const int& width, const int& height);
		static void DrawChar(CHAR_INFO* buffer, const int& width, const int& height, const int& x, const int& y, const short& character = PIXEL_SOLID, const short& colour = FG_WHITE);
		static void DrawRect(CHAR_INFO* buffer, const int& width, const int& height, const int& minX, const int& minY, const int& maxX, const int& maxY, const short& character = PIXEL_SOLID, const short& colour = FG_WHITE);
		static void DrawRectFill(CHAR_INFO* buffer, const int& width, const int& height, const int& minX, const int& minY, const int& maxX, const int& maxY, const short& borderCharacter = PIXEL_SOLID, const short& borderColour = FG_WHITE, const short& fillCharacter = ' ', const short& fillColour = FG_BLACK);
		static void DrawString(CHAR_INFO* buffer, const int& width, const int& height, const int& x, const int& y, const std::wstring& msg, const short& colour = FG_WHITE);
		static void DrawCircle(CHAR_INFO* buffer, const int& width, const int& height, const int& centreX, const int& centreY, const int& radius, const short& character = PIXEL_SOLID, const short& colour = FG_WHITE);
	};
}