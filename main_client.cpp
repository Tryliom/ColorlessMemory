#include "asset_manager.h"
#include "game.h"

int main()
{
	AssetManager::Initialize();
	Game::Initialize();

	return Game::Loop();
}