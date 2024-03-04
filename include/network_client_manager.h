#pragma once

#include "client.h"
#include "packet.h"

#include <functional>

class NetworkClientManager
{
private:
	bool running = true;
	std::function<bool(const Packet&)> onServerPacketReceived;

	void ReceivePackets(Client& client);
	void SendPackets(Client& client) const;

public:
	NetworkClientManager() = default;

	void SetOnMessageReceived(const std::function<bool(const Packet&)>& onMessageReceived);
	void StartThreads(Client& client);
};