#pragma once

#include "client.h"

#include <mutex>

class ClientManager
{
	std::vector<Client*> clients;
	mutable std::mutex mutex;

public:
	ClientManager() = default;

	~ClientManager();

	bool isEmpty() const;

	std::size_t AddClient(sf::TcpSocket* socket);
	void RemoveClient(std::size_t index);

	void Acknowledge(std::size_t index);

	/**
	 * @brief Check packet to be sent for all clients
	 */
	void CheckPacketToBeSent();
	void SendPacketToAllClients(sf::Packet* packet, sf::TcpSocket* sender = nullptr);

	Client* operator[](std::size_t index);
};