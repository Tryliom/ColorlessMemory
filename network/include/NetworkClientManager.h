#pragma once

#include "Client.h"
#include "Packet.h"

#include <functional>

class NetworkClientManager
{
private:
	bool _running = true;
	std::vector<Packet*> _packetReceived;
	mutable std::mutex _mutex;

	void ReceivePackets(Client& client);
	void SendPackets(Client& client) const;

public:
	NetworkClientManager() = default;

	void StartThreads(Client& client);

	Packet* PopPacket();
};