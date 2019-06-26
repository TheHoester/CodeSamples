#include "RenderEngine.h"

/**
 * Constructor
 */
RenderEngine::RenderEngine(int screenWidth, int screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight), bytesWritten(0)
{
	// Gets the Console Screen Buffer
	console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);

	// Gets and Sets the Console Font size
	CONSOLE_FONT_INFOEX cfi = { sizeof(CONSOLE_FONT_INFOEX) };
	GetCurrentConsoleFontEx(console, false, &cfi);
	cfi.dwFontSize.X = 8;
	cfi.dwFontSize.Y = 16;
	SetCurrentConsoleFontEx(console, false, &cfi);

	// Sets the size of the console to match the font size
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r);
	MoveWindow(console, r.left, r.top, (cfi.dwFontSize.X + 0.25) * screenWidth, (cfi.dwFontSize.Y - 0.5) * screenHeight, TRUE);
}

/**
 * Destructor
 */
RenderEngine::~RenderEngine()
{
	CloseHandle(console);
}

/**
 * Draw()
 * Takes an array of characters and draws them to the console.
 * @param characterArray The array of characters that will be drawn to the console.
 */
void RenderEngine::Draw(const wchar_t *characterArray)
{
	WriteConsoleOutputCharacter(console, characterArray, screenWidth * screenHeight, { 0,0 }, &bytesWritten);
}