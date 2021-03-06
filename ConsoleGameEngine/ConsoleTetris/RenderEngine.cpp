#include "RenderEngine.h"

using namespace Engine::Graphics;

/**
 * Destructor
 */
RenderEngine::~RenderEngine()
{
	if (console != NULL)
		CloseHandle(console);
}

/**
 * SetupWindow()
 * Sets the size of the console and the font used.
 * @param width The width of the console in characters.
 * @param height The height of the console in characters.
 * @param fontWidth The width of the characters in pixels.
 * @param fontHeight The height of the characters in pixels.
 */
void RenderEngine::SetupWindow(const int& width, const int& height, const int& fontWidth, const int& fontHeight)
{
	// Checks if this is the first time the window is setup.
	if (console == NULL)
	{
		// TODO: Error check console handle
		console = GetStdHandle(STD_OUTPUT_HANDLE);
		bytesWritten = 0;
	}

	screenWidth = width;
	screenHeight = height;

	// Change console visualsize to be the minimum so Screen Buffer can shrink.
	windowRect = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(console, TRUE, &windowRect); // TODO: Error check

	// Set size of Screen Buffer.
	COORD screenDimentions = { (short)screenWidth, (short)screenHeight };
	SetConsoleScreenBufferSize(console, screenDimentions); // TODO: Error check

	// Set Screen Buffer to console.
	SetConsoleActiveScreenBuffer(console); // TODO: Error check

	// Set the font size.
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = fontWidth;
	cfi.dwFontSize.Y = fontHeight;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	wcscpy_s(cfi.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(console, false, &cfi); // TODO: Error check

	// Checks if the dimentions exceed the maximum allows window size. 
	CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
	GetConsoleScreenBufferInfo(console, &screenBufferInfo); // TODO: Error check
	// TODO: Check if window size exceeds the maximum allowed dimentions
	// if (screenHeight > screenBufferInfo.Y)
	//		THROW ERROR
	// if (screenWidth > screenBufferInfo.X)
	//		THROW ERROR

	// Sets the window size
	windowRect = { 0, 0, (short)screenWidth - 1, (short)screenHeight - 1 };
	SetConsoleWindowInfo(console, TRUE, &windowRect); // TODO: Error check
}

/**
 * Draw()
 * Takes an array of characters and draws them to the console.
 * @param title The name of the program to be displayed on the top bar.
 * @param characterArray The array of characters that will be drawn to the console.
 */
void RenderEngine::Draw(const wchar_t* title, const CHAR_INFO* characterArray, const float& runTime, const float& renderTime)
{
	wchar_t s[256];
	swprintf_s(s, 256, L"%s - FPS: %3.2f - Run: %3.2f - Rend: %3.2f", title, 1.0f / Time::Instance().DeltaTime(), runTime, renderTime);
	SetConsoleTitle(s);
	WriteConsoleOutput(console, characterArray, { (short)screenWidth, (short)screenHeight }, { 0,0 }, &windowRect);
}