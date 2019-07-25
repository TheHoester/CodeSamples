#include "ArcadeGames.h"

/*
 * Constructor
 * @param name The name that will be displayed on the top bar.
 * @param width Character width of the screen.
 * @param height Character height of the screen.
 * @param width Pixel width of the font.
 * @param height Pixel height of the font.
 */
ArcadeGames::ArcadeGames(std::wstring name, int width, int height, int fontWidth, int fontHeight) : GameEngine(name, width, height, fontWidth, fontHeight) { }

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
	state = 0;
	gameStates = new Application*[numOfStates];
	gameStates[0] = new MainMenu(this, 0);
	gameStates[1] = new Tetris(this, 1);
	gameStates[2] = new Snake(this, 2);
	gameStates[3] = new FirstPerson(this, 3);
	gameStates[4] = new BouncingBall(this, 4);
	gameStates[5] = new CellularAutomata(this, 5);
	gameStates[6] = new AutoMaze(this, 6);
	gameStates[7] = new Racing(this, 7);
	gameStates[8] = new Frogger(this, 8);
	gameStates[9] = new ThreeDimentions(this, 9);
	gameStates[10] = new SideScroller(this, 10);

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
			UpdateWindow();
	}

	return true;
}

/*
 * UpdateScreenBuffer()
 * Makes sure that the applications all recieve the new pointer been the screen buffer is changed.
 * (Different apps require screens of varying sizes).
 */
void ArcadeGames::UpdateWindow()
{
	screenWidth = gameStates[state]->ScreenWidth();
	screenHeight = gameStates[state]->ScreenHeight();

	// Resets screen buffer ready for the new app
	if (screenBuffer != NULL)
		delete[] screenBuffer;

	screenBuffer = new CHAR_INFO[screenWidth * screenHeight];
	memset(screenBuffer, 0, sizeof(CHAR_INFO) * screenWidth * screenHeight);

	RenderEngine::Instance().SetupWindow(screenWidth, screenHeight, gameStates[state]->FontWidth(), gameStates[state]->FontHeight());
}