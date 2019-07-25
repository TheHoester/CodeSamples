#include "MainMenu.h"

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
MainMenu::MainMenu(GameEngine* engine, int appID, int width, int height, int fontWidth, int fontHeight) : Application(engine, appID, width, height, fontWidth, fontHeight),
	currentOption(1), inputDelayCounter(0), calculateInput(false) 
{
	GenerateAssets();
}

/**
 * Destructor
 */
MainMenu::~MainMenu() 
{ 
	if (menuOptions != NULL)
		delete[] menuOptions;
}

/*
 * Update()
 * Public call to the main update function for the game.
 * @return The ID of the game to change to if one is selected, -1 to close the program else will return the menus appID (0).
 */
int MainMenu::Update()
{
	GameLogic();
	Draw();

	if (InputHandler::Instance().IsKeyPressed(VK_ESCAPE))
		return -1;
	else if (InputHandler::Instance().IsKeyPressed(VK_RETURN))
		return currentOption;
	else
		return appID;
}

/*
 * GameLogic()
 * Runs the main logic for the game.
 */
void MainMenu::GameLogic()
{
	if (InputHandler::Instance().IsKeyPressed(VK_DOWN))
	{
		currentOption = (currentOption % numMenuOptions) + 1;
		inputDelayCounter = 0;
	}

	if (InputHandler::Instance().IsKeyPressed(VK_UP))
	{
		--currentOption;
		if (currentOption == 0)
			currentOption = numMenuOptions;

		inputDelayCounter = 0;
	}
}

/*
 * Draw()
 * Draws the game to the screen buffer.
 */
void MainMenu::Draw()
{
	engine->ClearScreen();

	// Draw Field
	engine->DrawRectFill(1, 1, menuWidth, menuHeight, '#');

	// Draw Options
	for (int i = 0; i < numMenuOptions; ++i)
		engine->DrawString(6, i + 3, menuOptions[i]);

	// Draw Arrow Cursor
	engine->DrawChar(4, currentOption + 2, '>');

	engine->DrawString(2, 20, L"Mouse X: " + std::to_wstring(InputHandler::Instance().GetMousePosition().x));
	engine->DrawString(2, 21, L"Mouse Y: " + std::to_wstring(InputHandler::Instance().GetMousePosition().y));

	if (InputHandler::Instance().IsMouseButtonHeld(0))
		engine->DrawString(2, 22, L"Left Mouse Button: True");
	else
		engine->DrawString(2, 22, L"Left Mouse Button: False");
}

/*
 * GenerateAssets()
 * Initilizes all the assets for the game.
 */
void MainMenu::GenerateAssets()
{
	menuOptions = new std::wstring[numMenuOptions];

	menuOptions[0] = L"Tetris";
	menuOptions[1] = L"Snake";
	menuOptions[2] = L"First Person";
	menuOptions[3] = L"Bouncing Ball";
	menuOptions[4] = L"Cellular Automata";
	menuOptions[5] = L"Auto-Maze";
	menuOptions[6] = L"Racing";
	menuOptions[7] = L"Frogger";
	menuOptions[8] = L"3D World";
	menuOptions[9] = L"Side-Scroller";

	int optionLength = 0;
	for (int i = 0; i < numMenuOptions; ++i)
		if (menuOptions[i].length() > optionLength)
			optionLength = menuOptions[i].length();

	menuWidth = optionLength + 8;
	menuHeight = numMenuOptions + 4;
}