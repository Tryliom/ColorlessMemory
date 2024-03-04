#pragma once

#include "PacketManager.h"
#include "ClientManager.h"

#include <atomic>
#include <functional>

class NetworkServerManager
{
	ClientManager clients;
	sf::TcpListener listener;
	std::function<bool(sf::TcpSocket*, Packet*)> onClientMessageReceived;
	mutable std::mutex mutex;

public:
	NetworkServerManager() = default;
	~NetworkServerManager();

	std::atomic<bool> running = true;

	bool Bind(unsigned short port);
	/**
	 * @brief Listen to packets from clients, return false to disconnect the client
	 * @param onMessageReceived Callback function to be called when a packet is received other than acknowledgement and invalid
	 */
	void ListenToClientPackets(std::function<bool(sf::TcpSocket*, Packet*)> onMessageReceived);

	/**
	 * @brief Start the threads for the server to accept new clients and send/receive packets
	 */
	void StartThreads();
	/**
	 * @brief Send a message to all clients
	 * @param message The message to be sent
	 * @param sender The client that sent the message, if any
	 */
	void SendMessageToAllClients(MessagePacket* message, sf::TcpSocket* sender = nullptr);
	/**
	 * @brief Send a message to a specific client
	 * @param clientIndex The index of the client to send the message to
	 */
	void ReceivePacketFromClient(std::size_t clientIndex);
};