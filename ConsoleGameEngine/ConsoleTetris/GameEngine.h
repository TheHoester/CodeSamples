#pragma once
#include <list>
#include <thread>

#include "Colour.h"
#include "GameObject.h"
#include "FVector2.h"
#include "InputHandler.h"
#include "RenderEngine.h"
#include "Sprite.h"
#include "Time.h"

using namespace Engine::Graphics;
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

		// GameObject Handling Functions;
		void RenderObjects(void);
	protected:
		std::wstring appName;
		int screenWidth;
		int screenHeight;
		CHAR_INFO* screenBuffer;
		bool close;

		std::list<GameObject*> objectPool;

		// Testing Time
		std::chrono::time_point<std::chrono::system_clock> beforeTime;
		std::chrono::time_point<std::chrono::system_clock> afterTime;

		float runGameTime = 0.0f;
		float renderTime = 0.0f;

		// Virtual Game Functions
		virtual bool CreateGame(void) = 0;
		virtual bool RunGame(void) = 0;
	public:
		GameEngine(std::wstring name = L"GameEngine", int width = 80, int height = 30, int fontWidth = 8, int fontHeight = 16);
		~GameEngine(void);

		// Startup game - generate all assets needed
		void Start(void);

		// GameObject Handling Functions
		GameObject* CreateGameObject(float x, float y, Sprite* sprite);
		GameObject* CreateGameObject(FVector2 position, Sprite* sprite);

		// Draw Functions
		void ClearScreen();
		void DrawChar(const int& x, const int& y, const short& character = PIXEL_SOLID, const short& colour = FG_WHITE);
		void DrawLine(int x0,  int y0, const int& x1, const int& y1, const short& character = PIXEL_SOLID, const short& colour = FG_WHITE);
		void DrawTriangle(const int& x0, const int& y0, const int& x1, const int& y1, const int& x2, const int& y2, const short& character = PIXEL_SOLID, const short& colour = FG_WHITE);
		void DrawFillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const short& character = PIXEL_SOLID, const short& colour = FG_WHITE);
		void DrawRect(const int& minX, const int& minY, const int& maxX, const int& maxY, const short& character = PIXEL_SOLID, const short& colour = FG_WHITE);
		void DrawRectFill(const int& minX, const int& minY, const int& maxX, const int& maxY, const short& borderCharacter = PIXEL_SOLID, const short& borderColour = FG_WHITE, const short& fillCharacter = ' ', const short& fillColour = FG_BLACK);
		void DrawString(const int& x, const int& y, const std::wstring& msg, const short& colour = FG_WHITE);
		void DrawStringAlpha(const int& x, const int& y, const std::wstring& msg, const short& colour = FG_WHITE);
		void DrawCircle(const int& centreX, const int& centreY, const int& radius, const short& character = PIXEL_SOLID, const short& colour = FG_WHITE);
		void DrawSprite(const int& x, const int& y, const Sprite& sprite);
		void DrawPartialSprite(const int& x, const int& y, const int& minX, const int& minY, const int& maxX, const int& maxY, const Sprite& sprite);

		CHAR_INFO GetGreyScaleColour(const float& lum);
		CHAR_INFO GetColour(const short& baseColour, const float& lum);
	};
}