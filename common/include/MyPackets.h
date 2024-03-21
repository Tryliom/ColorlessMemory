#pragma once

#include "Packet.h"
#include "PacketManager.h"

namespace MyPackets
{
	void RegisterMyPackets();

	enum class MyPacketType : char
	{
		LobbyInformation = static_cast<char>(PacketType::COUNT), // All custom packets should be after COUNT
		ChangeDeck,
		JoinLobby,
		LeaveLobby,
		StartGame,
		Turn,
		CardInformation,
		LeaveGame,
		COUNT
	};
}