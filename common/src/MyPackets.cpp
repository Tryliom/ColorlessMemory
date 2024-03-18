#include "MyPackets.h"

#include "PacketManager.h"
#include "MyPackets/LobbyInformationPacket.h"
#include "MyPackets/ChangeDeckPacket.h"
#include "MyPackets/JoinLobbyPacket.h"
#include "MyPackets/LeaveLobbyPacket.h"
#include "MyPackets/TurnPacket.h"
#include "MyPackets/CardInformationPacket.h"
#include "MyPackets/StartGamePacket.h"
#include "MyPackets/LeaveGamePacket.h"

namespace MyPackets
{
	void RegisterMyPackets()
	{
		PacketManager::RegisterPacketType(new LobbyInformationPacket());
		PacketManager::RegisterPacketType(new ChangeDeckPacket());
		PacketManager::RegisterPacketType(new JoinLobbyPacket());
		PacketManager::RegisterPacketType(new LeaveLobbyPacket());
		PacketManager::RegisterPacketType(new TurnPacket());
		PacketManager::RegisterPacketType(new CardInformationPacket());
		PacketManager::RegisterPacketType(new StartGamePacket());
		PacketManager::RegisterPacketType(new LeaveGamePacket());
	}
}
