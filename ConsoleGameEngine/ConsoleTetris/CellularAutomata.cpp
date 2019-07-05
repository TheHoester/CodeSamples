#include "CellularAutomata.h"

/*
 * Constructor
 * @param screenBuffer A pointer to the screenbuffer to be able to draw to.
 * @param input Pointer to the input handler.
 * @param time Pointer to the time handler.
 * @param appID The id of the application.
 * @param width Character width of the screen.
 * @param height Character height of the screen.
 * @param fontWidth Pixel width of the font.
 * @param fontHeight Pixel height of the font.
 */
CellularAutomata::CellularAutomata(CHAR_INFO* screenBuffer, InputHandler* input, Time* time, int appID, int width, int height, int fontWidth, int fontHeight) :
	Application(screenBuffer, input, time, appID, width, height, fontWidth, fontHeight)
{
	currentState = new bool[screenWidth * screenHeight];
	newState = new bool[screenWidth * screenHeight];

	memset(currentState, 0, sizeof(bool) * screenWidth * screenHeight);
	memset(newState, 0, sizeof(bool) * screenWidth * screenHeight);

	GenerateAssets();
}

/*
 * Destructor
 */
CellularAutomata::~CellularAutomata()
{
	if (currentState != NULL)
		delete[] currentState;
	if (newState != NULL)
		delete[] newState;
}

/*
 * Update()
 * Public call to the main update function for the game.
 * @return 0 is the game should exit to menu, else the game ID number.
 */
int CellularAutomata::Update()
{
	for (int x = 0; x < screenWidth; ++x)
	{
		for (int y = 0; y < screenHeight; ++y)
		{
			int count = GetNeighbors(x, y);
			if (currentState[(y * screenWidth) + x]) // If alive - check for over or under population
				newState[(y * screenWidth) + x] = (count == 2 || count == 3);
			else // If dead - check for 3 neighbors and breed
				newState[(y * screenWidth) + x] = count == 3;

			// Draw Cell
			if (newState[(y * screenWidth) + x])
				GameEngine::DrawChar(screenBuffer, screenWidth, screenHeight, x, y);
			else
				GameEngine::DrawChar(screenBuffer, screenWidth, screenHeight, x, y, ' ');
		}
	}

	memcpy(currentState, newState, sizeof(bool) * screenWidth * screenHeight);

	if (input->IsKeyPressed(VK_RETURN))
		Reset();
	if (input->IsKeyPressed(VK_ESCAPE))
	{
		Reset();
		return 0;
	}

	return appID;
}

void CellularAutomata::GameLogic() { }
void CellularAutomata::Draw() { }

/*
 * Reset()
 * Resets the game back to the beginning state.
 */
void CellularAutomata::Reset()
{
	if (currentState != NULL)
		delete[] currentState;
	if (newState != NULL)
		delete[] newState;

	currentState = new bool[screenWidth * screenHeight];
	newState = new bool[screenWidth * screenHeight];

	memset(currentState, 0, sizeof(bool) * screenWidth * screenHeight);
	memset(newState, 0, sizeof(bool) * screenWidth * screenHeight);

	GenerateAssets();
}

/*
 * GenerateAssets()
 * Initilizes all the assets for the game.
 */
void CellularAutomata::GenerateAssets()
{
	for (int i = 0; i < screenWidth * screenHeight; ++i)
		currentState[i] = rand() % 2;
}

/*
 * GetNeighbors()
 * Finds out how many neighbors are alive from the 8 that surround the cell at the coordinates given.
 * @param x The x coordinate of the cell to be checked.
 * @param y The y coordinate of the cell to be checked.
 * @return Number of neighbors that are alive.
 */
int CellularAutomata::GetNeighbors(const int& x, const int& y)
{
	int count = 0;

	if (x > 0               && y > 0                && currentState[((y - 1) * screenWidth) + (x - 1)]) ++count; // Top Left
	if (                       y > 0                && currentState[((y - 1) * screenWidth) +  x     ]) ++count; // Top Middle
	if (x < screenWidth - 1 && y > 0                && currentState[((y - 1) * screenWidth) + (x + 1)]) ++count; // Top Right
	if (x > 0               &&                         currentState[( y      * screenWidth) + (x - 1)]) ++count; // Middle Left
	if (x < screenWidth - 1 &&                         currentState[( y      * screenWidth) + (x + 1)]) ++count; // Middle Right
	if (x > 0               && y < screenHeight - 1 && currentState[((y + 1) * screenWidth) + (x - 1)]) ++count; // Bottom Left
	if (                       y < screenHeight - 1 && currentState[((y + 1) * screenWidth) +  x     ]) ++count; // Bottom Middle
	if (x < screenWidth - 1 && y < screenHeight - 1 && currentState[((y + 1) * screenWidth) + (x + 1)]) ++count; // Bottom Right

	return count;
}