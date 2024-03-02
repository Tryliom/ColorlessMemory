#pragma once

#include <SFML/Network.hpp>

#include <queue>
#include <shared_mutex>

struct Client
{
	constexpr static sf::Int32 ACK_TIMEOUT = 500;
	sf::TcpSocket* socket = new sf::TcpSocket();
	sf::Packet* packetWaitingForAcknowledgement = nullptr;
	std::queue<sf::Packet*> packetsToBeSent = std::queue<sf::Packet*>();
	bool acknowledged = true;
	sf::Clock ackClock = sf::Clock();
	mutable std::shared_mutex mutex_;

	~Client()
	{
		delete packetWaitingForAcknowledgement;
	}

	void SendPacket(sf::Packet* packet)
	{
		std::unique_lock lock(mutex_);
		packetsToBeSent.push(packet);
	}

	bool IsPacketsEmpty() const
	{
		std::shared_lock lock(mutex_);
		return packetsToBeSent.empty();
	}
};
