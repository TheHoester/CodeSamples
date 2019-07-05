#include "ArcadeGames.h"

int main()
{
	ArcadeGames* game = new ArcadeGames();
	game->Start();
	delete game;
	return 0;
}