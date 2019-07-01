#include "Application.h"
#include "BouncingBall.h"
#include "Defines.h"
#include "FirstPerson.h"
#include "MainMenu.h"
#include "RenderEngine.h"
#include "Snake.h"
#include "Tetris.h"
#include "Time.h"

#include <chrono>
#include <vector>
#include <Windows.h>

const int screenWidth = 120;
const int screenHeight = 40;

const int mapWidth = 16;
const int mapHeight = 16;


int main()
{
	RenderEngine* renderer = new RenderEngine(SMALL_SCREEN_WIDTH, SMALL_SCREEN_HEIGHT);
	Time* time = new Time();
	Application** gameStates = new Application*[5];
	gameStates[0] = new MainMenu(renderer, time, SMALL_SCREEN_WIDTH, SMALL_SCREEN_HEIGHT);
	gameStates[1] = new Tetris(renderer, time, SMALL_SCREEN_WIDTH, SMALL_SCREEN_HEIGHT, TETRIS_FIELD_WIDTH, TETRIS_FIELD_HEIGHT);
	gameStates[2] = new Snake(renderer, time, SMALL_SCREEN_WIDTH, SMALL_SCREEN_HEIGHT, SNAKE_FIELD_WIDTH, SNAKE_FIELD_HEIGHT);
	gameStates[3] = new FirstPerson(renderer, time, LARGE_SCREEN_WIDTH, LARGE_SCREEN_HEIGHT);
	gameStates[4] = new BouncingBall(renderer, time, LARGE_SCREEN_WIDTH, LARGE_SCREEN_HEIGHT);

	int currentState = 0;
	bool gameOver = false;
	int score = 0;

	while (!gameOver || currentState != 0)
	{
		time->Update();
		if (!gameOver)
		{
			gameOver = gameStates[currentState]->Update();
			if (currentState == 0 && InputHandler::IsKeyDown(VK_RETURN))
			{
				currentState = gameStates[currentState]->GetScore();
				renderer->Reset(gameStates[currentState]->GetScreenWidth(), gameStates[currentState]->GetScreenHeight());
			}
		}

		if (gameOver && currentState != 0)
		{
			if (InputHandler::IsKeyDown(VK_RETURN))
			{
				gameStates[currentState]->Reset();
				gameOver = false;
			}
			if (InputHandler::IsKeyDown(VK_BACK) || currentState == 3 || currentState == 4)
			{
				gameStates[currentState]->Reset();
				gameOver = false;
				currentState = 0;
				renderer->Reset(gameStates[currentState]->GetScreenWidth(), gameStates[currentState]->GetScreenHeight());
			}
		}
	}

	for (int i = 0; i < 3; ++i)
		delete gameStates[i];
	delete[] gameStates;
	delete renderer;

	return 0;
}