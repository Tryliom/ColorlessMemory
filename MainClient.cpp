#include "AssetManager.h"
#include "Game.h"

int main()
{
	AssetManager::Initialize();
	Game::Initialize();

	return Game::Loop();
}