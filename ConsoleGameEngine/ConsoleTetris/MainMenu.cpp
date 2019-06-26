#include "MainMenu.h"

/**
 * Constructor
 * @param rend The render engine used to draw to the console.
 * @param screenWidth Character width of the screen.
 * @param screenHeight Character height of the screen.
 */
MainMenu::MainMenu(RenderEngine* rend, int screenWidth, int screenHeight) : Application(rend, screenWidth, screenHeight)
{
	keys = new bool[3];
	inputDelay = 8;
	inputDelayCounter = 0;
	calculateInput = false;

	score = 1;
	numMenuOptions = 2;

	GenerateAssets();
}

/**
 * Destructor
 */
MainMenu::~MainMenu() { }

/**
 * Update()
 * Handles running all the logic for the menu.
 * @return The current gameover state.
 */
bool MainMenu::Update()
{
	this_thread::sleep_for(25ms);
	++inputDelayCounter;
	calculateInput = (inputDelayCounter >= inputDelay);

	InputHandler();
	GameLogic();
	Draw();

	return gameOver;
}

/**
 * InputHandler()
 * Handles getting the current state of all the input.
 */
void MainMenu::InputHandler()
{
	keys[0] = InputHandler::IsKeyDown(VK_DOWN);
	keys[1] = InputHandler::IsKeyDown(VK_UP);
	keys[2] = InputHandler::IsKeyDown(VK_ESCAPE);
}

/**
 * GameLogic()
 * Handles the changing of state depending on the input given.
 */
void MainMenu::GameLogic()
{
	if (calculateInput)
	{
		if (keys[0])
		{
			score = (score % numMenuOptions) + 1;
			inputDelayCounter = 0;
		}
		if (keys[1])
		{
			--score;
			if (score == 0)
				score = numMenuOptions;

			inputDelayCounter = 0;
		}
		if (keys[2])
			gameOver = true;

	}
}

/**
 * Draw()
 * Handles drawing the menu to the console.
 */
void MainMenu::Draw()
{
	// Draw Field
	for (int x = 0; x < menuWidth; ++x)
		for (int y = 0; y < menuHeight; ++y)
			screen[((y + 2) * screenWidth) + (x + 2)] = L" #"[field[(y * menuWidth) + x]];

	// Draw Options
	for (int i = 0; i < numMenuOptions; ++i)
		swprintf_s(&screen[((4 + i) * screenWidth) + 7], menuOptions[i].length() + 1, menuOptions[i].c_str());

	// Arrow Cursor
	screen[((3 + score) * screenWidth) + 5] = L'>';

	// Display Frame
	renderer->Draw(screen);
}

/**
 * GenerateAssets()
 * Used to populate all variables with the assets needed to draw the menu.
 * Usually called in the constructor.
 */
void MainMenu::GenerateAssets()
{
	menuOptions[0] = L"Tetris";
	menuOptions[1] = L"Snake";

	int optionLength = 0;
	for (auto& option : menuOptions)
		if (option.length() > optionLength)
			optionLength = option.length();

	menuWidth = optionLength + 8;
	menuHeight = numMenuOptions + 4;

	field = Application::GenerateFieldBox(menuWidth, menuHeight, 1);
}