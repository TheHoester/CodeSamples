#include "Application.h"

/**
 * Constructor
 * @param buffer A pointer to the screenbuffer to be able to draw to.
 * @param input Pointer to the input handler.
 * @param time Pointer to the time handler.
 * @param appID The id of the application.
 * @param width Character width of the screen.
 * @param height Character height of the screen.
 * @param fontWidth Pixel width of the font.
 * @param fontHeight Pixel height of the font.
 */
Application::Application(GameEngine* engine, int appID, int width, int height, int fontWidth, int fontHeight) : engine(engine), 
	appID(appID), screenWidth(width), screenHeight(height), fontWidth(fontWidth), fontHeight(fontHeight) { }

/**
 * Destructor
 */
Application::~Application() { }

int Application::ScreenWidth() const { return screenWidth; }
int Application::ScreenHeight() const { return screenHeight; }
int Application::FontWidth() const { return fontWidth; }
int Application::FontHeight() const { return fontHeight; }

/**
 * Reset()
 * Resets the app back to it's starting state.
 */
void Application::Reset() { }

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