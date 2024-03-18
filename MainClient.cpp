#include "AssetManager.h"
#include "Game.h"
#include "MyPackets.h"

int main()
{
	MyPackets::RegisterMyPackets();
	AssetManager::Initialize();
	Game::Initialize();

	return Game::StartLoop();
}