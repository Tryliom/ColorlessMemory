#pragma once

#include <SFML/Network.hpp>

#include <queue>

struct Client
{
	constexpr static sf::Int32 ACK_TIMEOUT = 500;
	sf::TcpSocket* socket = new sf::TcpSocket();
	sf::Packet* packetWaitingForAcknowledgement = nullptr;
	std::queue<sf::Packet*> packetsToBeSent = std::queue<sf::Packet*>();
	bool acknowledged = true;
	sf::Clock ackClock = sf::Clock();

	~Client()
	{
		delete packetWaitingForAcknowledgement;
	}

	void SendPacket(sf::Packet* packet)
	{
		packetsToBeSent.push(packet);
	}
};
