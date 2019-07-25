#include "SpriteEditor.h"
/*
 * Constructor
 * @param name The name that will be displayed on the top bar.
 * @param width Character width of the screen.
 * @param height Character height of the screen.
 * @param width Pixel width of the font.
 * @param height Pixel height of the font.
 */
SpriteEditor::SpriteEditor(std::wstring name, int width, int height, int fontWidth, int fontHeight) : GameEngine(name, width, height, fontWidth, fontHeight),
	currentPixel(PIXEL_SOLID), currentColour(9), currentBrightness(11) { }

/*
 * Destructor
 */
SpriteEditor::~SpriteEditor()
{
	if (sprite != nullptr)
		delete sprite;
}

/*
 * CreateGame()
 * Initilizes all the elements of the editor.
 * @return Will return false if an error occurs.
 */
bool SpriteEditor::CreateGame()
{
	filename = L"../Assets/SideScroller/Player1.spr";
	sprite = new Sprite(16, 16);
	cellSize = 8;
	
	cursorPosition = Vector2(0, 0);
	menuOffset = 10;

	return true;
}

/*
 * RunGame()
 * The logic for the editor.
 * @return Will return false if an error occurs.
 */
bool SpriteEditor::RunGame()
{
	Input();
	
	// DRAW ###########################################################################################################################################
	ClearScreen();

	// Current Selected Pixel and Colour
	for (int i = 0; i < 7; ++i)
	{
		short colour = i + 9;
		CHAR_INFO colourInfo;
		if (colour == 15)
			colourInfo = GetGreyScaleColour(currentBrightness / 19.0f);
		else
			colourInfo = GetColour(colour, currentBrightness / 19.0f);

		if (colour == currentColour)
			DrawRectFill((i * 10) + 1, 1, (i * 10) + 7, 7, PIXEL_SOLID, FG_WHITE, colourInfo.Char.UnicodeChar, colourInfo.Attributes);
		else
			DrawRectFill((i * 10) + 1, 1, (i * 10) + 7, 7, PIXEL_SOLID, FG_BLACK, colourInfo.Char.UnicodeChar, colourInfo.Attributes);

	}

	// Points on cell corners and sprite
	for (int x = 0; x < sprite->Width(); ++x)
	{
		for (int y = 0; y < sprite->Height(); ++y)
		{
			if (sprite->GetPixel(x, y) != 0)
				DrawRectFill(x * cellSize, (y * cellSize) + menuOffset, ((x + 1) * cellSize) - 1, ((y + 1) * cellSize) - 1 + menuOffset, 
					sprite->GetPixel(x, y), sprite->GetColour(x, y), sprite->GetPixel(x, y), sprite->GetColour(x, y));
			else
			{
				DrawChar(x * cellSize, (y * cellSize) + menuOffset);
				DrawChar(((x + 1) * cellSize) - 1, (y * cellSize) + menuOffset);
				DrawChar(x * cellSize, ((y + 1) * cellSize) - 1 + menuOffset);
				DrawChar(((x + 1) * cellSize) - 1, ((y + 1) * cellSize) - 1 + menuOffset);
			}
		}
	}

	// Cursor
	DrawRectFill((cursorPosition.x * cellSize) + (cellSize / 2) - 1, (cursorPosition.y * cellSize) + (cellSize / 2) - 1 + menuOffset,
				(cursorPosition.x * cellSize) + (cellSize / 2), (cursorPosition.y * cellSize) + (cellSize / 2) + menuOffset,
				PIXEL_SOLID, FG_YELLOW, PIXEL_SOLID, FG_YELLOW);

	return true;
}

void SpriteEditor::Input()
{
	// Exit
	if (InputHandler::Instance().IsKeyPressed(VK_ESCAPE))
		close = true;

	// Move Cursor
	if (InputHandler::Instance().IsKeyPressed('W'))
		--cursorPosition.y;
	if (InputHandler::Instance().IsKeyPressed('A'))
		--cursorPosition.x;
	if (InputHandler::Instance().IsKeyPressed('S'))
		++cursorPosition.y;
	if (InputHandler::Instance().IsKeyPressed('D'))
		++cursorPosition.x;

	if (cursorPosition.x < 0)
		cursorPosition.x = sprite->Width() - 1;
	else if (cursorPosition.x >= sprite->Width())
		cursorPosition.x = 0;
	if (cursorPosition.y < 0)
		cursorPosition.y = sprite->Height() - 1;
	else if (cursorPosition.y >= sprite->Height())
		cursorPosition.y = 0;

	// Adds Pixel
	if (InputHandler::Instance().IsKeyPressed(VK_RETURN))
	{
		CHAR_INFO info = GetColour(currentColour, currentBrightness / 19.0f);
		sprite->SetPixel(cursorPosition.x, cursorPosition.y, info.Char.UnicodeChar);
		sprite->SetColour(cursorPosition.x, cursorPosition.y, info.Attributes);
	}

	// Deletes Pixel
	if (InputHandler::Instance().IsKeyPressed(VK_DELETE))
	{
		sprite->SetPixel(cursorPosition.x, cursorPosition.y, 0);
		sprite->SetColour(cursorPosition.x, cursorPosition.y, 0);
	}

	// Select Colours
	if (InputHandler::Instance().IsKeyPressed(VK_LEFT))
		--currentColour;
	if (InputHandler::Instance().IsKeyPressed(VK_RIGHT))
		++currentColour;

	if (currentColour < 9)
		currentColour = 15;
	else if (currentColour > 15)
		currentColour = 9;

	// Select Brightness
	if (InputHandler::Instance().IsKeyPressed(VK_UP))
		++currentBrightness;
	if (InputHandler::Instance().IsKeyPressed(VK_DOWN))
		--currentBrightness;

	if (currentBrightness < 0)
		currentBrightness = 19;
	else if (currentBrightness > 19)
		currentBrightness = 0;

	// Zoom
	if (InputHandler::Instance().IsKeyPressed('O'))
		cellSize += 2;
	if (InputHandler::Instance().IsKeyPressed('P'))
		cellSize -= 2;

	if (cellSize > 16)
		cellSize = 16;
	if (cellSize < 4)
		cellSize = 4;

	// Save
	if (InputHandler::Instance().IsKeyPressed('S'))
		sprite->Save(filename);

	// Load
	if (InputHandler::Instance().IsKeyPressed('L'))
		sprite->Load(filename);
}