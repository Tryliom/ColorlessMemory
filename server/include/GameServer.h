#pragma once

#include "NetworkServerManager.h"

#include <SFML/Network.hpp>

struct LobbyData
{
	sf::TcpSocket* player1;
	sf::TcpSocket* player2;
};

namespace GameServer
{
	void Initialize();
	void StartLoop();
};