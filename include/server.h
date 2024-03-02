#pragma once

#include "packetManager.h"
#include "clientManager.h"

#include <atomic>

class Server
{
	ClientManager clients;
	sf::TcpListener listener;
	mutable std::mutex mutex;

public:
	Server() = default;
	~Server();

	std::atomic<bool> running = true;

	bool Bind(unsigned short port);

	/**
	 * @brief Start the threads for the server to accept new clients and send/receive packets
	 */
	void StartThreads();
	void SendMessageToAllClients(const MessagePacket& message, sf::TcpSocket* sender = nullptr);
	void ReceivePacketFromClient(std::size_t clientIndex);
};