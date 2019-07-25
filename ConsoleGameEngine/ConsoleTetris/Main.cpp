#include "ArcadeGames.h"
#include "SpriteEditor.h"

int main()
{
	if (true) 
	{
		ArcadeGames* game = new ArcadeGames();
		game->Start();
		delete game;
	}
	else
	{
		SpriteEditor* editor = new SpriteEditor();
		editor->Start();
		delete editor;
	}

	return 0;
}