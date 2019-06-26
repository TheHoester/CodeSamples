#include "Application.h"

/**
 * Constructor
 * @param rend The render engine used to draw to the console.
 * @param screenWidth Character width of the screen.
 * @param screenHeight Character height of the screen.
 */
Application::Application(RenderEngine* rend, int screenWidth, int screenHeight): renderer(rend), screenWidth(screenWidth), screenHeight(screenHeight)
{
	screen = new wchar_t[screenWidth * screenHeight];
	for (int i = 0; i < screenWidth * screenHeight; ++i)
		screen[i] = L' ';

	gameOver = false;
	score = 0;
}

/**
 * Destructor
 */
Application::~Application() 
{
	delete[] field;
	delete[] screen;
	delete[] keys;
}
/**
 * GetScore()
 * @return The games score.
 */
int Application::GetScore() const { return score; }

/**
 * Reset()
 * Resets the app back to it's starting state.
 */
void Application::Reset()
{
	delete[] field;
	delete[] screen;
	screen = new wchar_t[screenWidth * screenHeight];
	for (int i = 0; i < screenWidth * screenHeight; ++i)
		screen[i] = L' ';

	gameOver = false;
	score = 0;
}

/**
 * GenerateFieldBox()
 * Creates a full box around the border of the play field.
 * @param fieldWidth The character width of the play field.
 * @param fieldHeight The character height of the play field.
 * @param character Integer representation of the character used to build the box.
 * @return A char array of the play field with a box outlining the border of the field.
 */
unsigned char* Application::GenerateFieldBox(int fieldWidth, int fieldHeight, int character)
{
	unsigned char* field = new unsigned char[fieldWidth * fieldHeight];
	for (int x = 0; x < fieldWidth; ++x)
		for (int y = 0; y < fieldHeight; ++y)
			field[(y * fieldWidth) + x] = (x == 0 || y == 0 || x == fieldWidth - 1 || y == fieldHeight - 1) ? character : 0;

	return field;
}

/**
 * GenerateFieldOpenTopBox()
 * Creates an open topped box around the border of the play field.
 * @param fieldWidth The character width of the play field.
 * @param fieldHeight The character height of the play field.
 * @param character Integer representation of the character used to build the box.
 * @return A char array of the play field with an open topped box outlining the border of the field.
 */
unsigned char* Application::GenerateFieldOpenTopBox(int fieldWidth, int fieldHeight, int character)
{
	unsigned char* field = new unsigned char[fieldWidth * fieldHeight];
	for (int x = 0; x < fieldWidth; ++x)
		for (int y = 0; y < fieldHeight; ++y)
			field[(y * fieldWidth) + x] = (x == 0 || x == fieldWidth - 1 || y == fieldHeight - 1) ? character : 0;

	return field;
}