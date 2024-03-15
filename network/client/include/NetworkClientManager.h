#pragma once

#include "Client.h"
#include "../../common/include/Packet.h"

#include <functional>

//TODO: Network Interface -> Send/receive packet

/**
 * Receive and send packet from/to server
 */
class NetworkClientManager
{
private:
	bool _running = true;
	std::queue<Packet*> _packetReceived;
	mutable std::shared_mutex _mutex;

	// Launch from a thread
	void ReceivePackets(Client& client);
	void SendPackets(Client& client) const;

	//TODO: Add isEmpty function with shared_lock and put him before PopPacket

public:
	NetworkClientManager() = default;

	void StartThreads(Client& client);

	Packet* PopPacket();

	void Stop() { _running = false; }
};