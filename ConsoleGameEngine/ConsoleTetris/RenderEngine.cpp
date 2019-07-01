#include "RenderEngine.h"

/**
 * Constructor
 */
RenderEngine::RenderEngine(int screenWidth, int screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight), bytesWritten(0)
{
	// Small window = Width = 656, Height = 471
	// Large window = Width = 976, Height = 681

	// Gets the Console Screen Buffer
	console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);

	// Sets the window size
	CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
	SMALL_RECT windowRect;
	GetConsoleScreenBufferInfo(console, &screenBufferInfo);
	windowRect = screenBufferInfo.srWindow;
	windowRect.Right = windowRect.Left + screenWidth - 1;
	windowRect.Bottom = windowRect.Top + screenHeight - 1;
	SetConsoleWindowInfo(console, true, &windowRect);

	// Set screen buffer size
	COORD screenDimentions;
	screenDimentions.X = screenWidth;
	screenDimentions.Y = screenHeight;
	SetConsoleScreenBufferSize(console, screenDimentions);

	HWND hConsole = GetConsoleWindow();
	RECT r;
	GetWindowRect(hConsole, &r);
	MoveWindow(hConsole, r.left, r.top, (screenWidth + 2) * 8, (screenHeight + 3) * 16, TRUE);

	// Gets and Sets the Console Font size
	CONSOLE_FONT_INFOEX cfi = { sizeof(CONSOLE_FONT_INFOEX) };
	GetCurrentConsoleFontEx(console, false, &cfi);
	cfi.dwFontSize.X = 8;
	cfi.dwFontSize.Y = 16;
	SetCurrentConsoleFontEx(console, false, &cfi);
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

void RenderEngine::Reset(int width, int height)
{
	screenWidth = width;
	screenHeight = height;
	
	// Sets the window size
	CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
	SMALL_RECT windowRect;
	GetConsoleScreenBufferInfo(console, &screenBufferInfo);
	windowRect = screenBufferInfo.srWindow;
	windowRect.Right = windowRect.Left + screenWidth - 1;
	windowRect.Bottom = windowRect.Top + screenHeight - 1;
	SetConsoleWindowInfo(console, true, &windowRect);

	// Set screen buffer size
	COORD screenDimentions;
	screenDimentions.X = screenWidth;
	screenDimentions.Y = screenHeight;
	SetConsoleScreenBufferSize(console, screenDimentions);

	HWND hConsole = GetConsoleWindow();
	RECT r;
	GetWindowRect(hConsole, &r);
	MoveWindow(hConsole, r.left, r.top, (screenWidth + 2) * 8, (screenHeight + 3) * 16, TRUE);

	// Gets and Sets the Console Font size
	CONSOLE_FONT_INFOEX cfi = { sizeof(CONSOLE_FONT_INFOEX) };
	GetCurrentConsoleFontEx(console, false, &cfi);
	cfi.dwFontSize.X = 8;
	cfi.dwFontSize.Y = 16;
	SetCurrentConsoleFontEx(console, false, &cfi);
}