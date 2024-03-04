#pragma once

#include "Packet.h"

#include <SFML/Network.hpp>

#include <queue>
#include <shared_mutex>
#include <mutex>

struct Client
{
	sf::TcpSocket* socket = new sf::TcpSocket();
	std::queue<Packet*> packetsToBeSent = std::queue<Packet*>();
	mutable std::shared_mutex mutex_;

	void SendPacket(Packet* packet)
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
