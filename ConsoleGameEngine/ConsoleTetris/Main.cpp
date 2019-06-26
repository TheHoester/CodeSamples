#include "Application.h"
#include "Defines.h"
#include "MainMenu.h"
#include "RenderEngine.h"
#include "Snake.h"
#include "Tetris.h"

int main()
{
	RenderEngine* renderer = new RenderEngine(SCREEN_WIDTH, SCREEN_HEIGHT);
	Application** gameStates = new Application*[3];
	gameStates[0] = new MainMenu(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	gameStates[1] = new Tetris(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, TETRIS_FIELD_WIDTH, TETRIS_FIELD_HEIGHT);
	gameStates[2] = new Snake(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, SNAKE_FIELD_WIDTH, SNAKE_FIELD_HEIGHT);

	int currentState = 0;
	bool gameOver = false;
	int score = 0;

	while (!gameOver || currentState != 0)
	{
		if (!gameOver)
		{
			gameOver = gameStates[currentState]->Update();
			if (currentState == 0 && InputHandler::IsKeyDown(VK_RETURN))
				currentState = gameStates[currentState]->GetScore();
		}

		if (gameOver && currentState != 0)
		{
			if (InputHandler::IsKeyDown(VK_RETURN))
			{
				gameStates[currentState]->Reset();
				gameOver = false;
			}
			if (InputHandler::IsKeyDown(VK_BACK))
			{
				gameStates[currentState]->Reset();
				gameOver = false;
				currentState = 0;
			}
		}
	}

	for (int i = 0; i < 3; ++i)
		delete gameStates[i];
	delete[] gameStates;
	delete renderer;

	return 0;
}