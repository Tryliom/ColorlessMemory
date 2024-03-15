#pragma once

#include "PacketManager.h"
#include "ClientManager.h"

#include <atomic>
#include <functional>

class NetworkServerManager
{
	ClientManager _clients;
	sf::TcpListener _listener;
	std::function<void(sf::TcpSocket*, Packet*)> _onPacketReceived;
	std::function<void(sf::TcpSocket*)> _onDisconnect;
	mutable std::mutex _mutex;

public:
	NetworkServerManager() = default;
	~NetworkServerManager();

	std::atomic<bool> Running = true;

	bool Bind(unsigned short port);
	/**
	 * @brief Listen to packets from clients, return false to disconnect the client
	 * @param onPacketReceived Callback function to be called when a packet is received other than acknowledgement and invalid
	 */
	void OnPacketReceived(std::function<void(sf::TcpSocket*, Packet*)> onPacketReceived);
	void OnDisconnect(std::function<void(sf::TcpSocket*)> onDisconnect);

	/**
	 * @brief Start the threads for the server to accept new clients and send/receive packets
	 */
	void StartThreads();
	/**
	 * @brief Send a message to a specific client
	 * @param clientIndex The index of the client to send the message to
	 */
	void ReceivePacketFromClient(std::size_t clientIndex);
};