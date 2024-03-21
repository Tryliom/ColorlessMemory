#pragma once

#include "PacketManager.h"
#include "ClientManager.h"
#include "ServerNetworkInterface.h"

#include <atomic>
#include <functional>

class NetworkServerManager final : ServerNetworkInterface
{
	ClientManager _clients;
	sf::TcpListener _listener;
	std::function<void(PacketData)> _onPacketReceived;
	std::function<void(ClientId)> _onDisconnect;
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
	void OnPacketReceived(std::function<void(PacketData)> onPacketReceived);
	void OnDisconnect(std::function<void(ClientId)> onDisconnect);

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