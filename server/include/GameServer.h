#pragma once

#include "NetworkServerManager.h"

#include <SFML/Network.hpp>

struct LobbyData
{
	sf::TcpSocket* player1;
	sf::TcpSocket* player2;
	std::string player1Name;
	std::string player2Name;
	std::size_t player1Icon;
	std::size_t player2Icon;

	[[nodiscard]] LobbyInformationPacket* ToPacket(bool isHost) const
	{
		return new LobbyInformationPacket(isHost, player1 == nullptr || player2 == nullptr, player1Name, player2Name, player1Icon, player2Icon);
	}

	void Reset()
	{
		player1 = nullptr;
		player2 = nullptr;
		player1Name.clear();
		player2Name.clear();
		player1Icon = 0;
		player2Icon = 0;
	}
};

namespace GameServer
{
	void Initialize();
	void StartLoop();
};