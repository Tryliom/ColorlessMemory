#include "GameServer.h"

int main()
{
	GameServer::Initialize();
	GameServer::StartLoop();

	return EXIT_SUCCESS;
}