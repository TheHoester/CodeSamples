#pragma once
#include "GameEngine.h"
#include "Sprite.h"

using namespace Engine::Graphics;

class SpriteEditor : public Engine::GameEngine
{
private:
	Sprite* sprite;
	std::wstring filename;

	int cellSize;
	int menuOffset;

	Vector2 cursorPosition;
	short currentPixel;
	short currentColour;
	short currentBrightness;

	// Overridden Functions
	bool CreateGame(void) override;
	bool RunGame(void) override;

	// Misc Functions
	void Input(void);

public:
	SpriteEditor(std::wstring name = L"Sprite Editor", int width = 320, int height = 160, int fontWidth = 4, int fontHeight = 4);
	~SpriteEditor(void);
};