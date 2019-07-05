#include "ArcadeGames.h"

/*
 * Constructor
 * @param name The name that will be displayed on the top bar.
 * @param width Character width of the screen.
 * @param height Character height of the screen.
 * @param width Pixel width of the font.
 * @param height Pixel height of the font.
 */
ArcadeGames::ArcadeGames(std::wstring name, int width, int height, int fontWidth, int fontHeight) : GameEngine(name, width, height, fontWidth, fontHeight), state(0) { }

/*
 * Destructor
 */
ArcadeGames::~ArcadeGames()
{
	if (gameStates != NULL)
	{
		for (int i = 0; i < numOfStates; ++i)
			if (gameStates[i] != NULL)
				delete gameStates[i];
		delete[] gameStates;
	}
}

/*
 * CreateGame()
 * Initilizes all the elements of the game.
 * @return Will return false if an error occurs.
 */
bool ArcadeGames::CreateGame()
{
	gameStates = new Application*[7];

	gameStates[0] = new MainMenu(screenBuffer, inputHandler, time, 0);
	gameStates[1] = new Tetris(screenBuffer, inputHandler, time, 1);
	gameStates[2] = new Snake(screenBuffer, inputHandler, time, 2);
	gameStates[3] = new FirstPerson(screenBuffer, inputHandler, time, 3);
	gameStates[4] = new BouncingBall(screenBuffer, inputHandler, time, 4);
	gameStates[5] = new CellularAutomata(screenBuffer, inputHandler, time, 5);
	gameStates[6] = new AutoMaze(screenBuffer, inputHandler, time, 6);

	return true;
}

/*
 * RunGame()
 * The game logic for the game.
 * @return Will return false if an error occurs.
 */
bool ArcadeGames::RunGame()
{
	int changeState = gameStates[state]->Update();

	if (changeState != state)
	{
		state = changeState;
		if (state == -1)
			close = true;
		else
		{
			UpdateScreenBuffer();
			memset(screenBuffer, 0, sizeof(CHAR_INFO) * gameStates[state]->ScreenWidth() * gameStates[state]->ScreenHeight());
			renderer->SetupWindow(gameStates[state]->ScreenWidth(), gameStates[state]->ScreenHeight(), gameStates[state]->FontWidth(), gameStates[state]->FontHeight());
		}
	}

	return true;
}

/*
 * UpdateScreenBuffer()
 * Makes sure that the applications all recieve the new pointer been the screen buffer is changed.
 * (Different apps require screens of varying sizes).
 */
void ArcadeGames::UpdateScreenBuffer()
{
	// Resets screen buffer ready for the new app
	if (screenBuffer != NULL)
		delete[] screenBuffer;

	screenBuffer = new CHAR_INFO[gameStates[state]->ScreenWidth() * gameStates[state]->ScreenHeight()];

	for (int i = 0; i < 7; ++i)
		gameStates[i]->SetScreenBuffer(screenBuffer);
}