#include "GameServer.h"
#include "PacketManager.h"
#include "MyPackets.h"

int main()
{
	MyPackets::RegisterMyPackets();
	GameServer::Initialize();
	GameServer::StartLoop();

	return EXIT_SUCCESS;
}