#pragma once

#include "client.h"
#include "packet.h"

#include <functional>

class NetworkClientManager
{
private:
	bool running = true;
	std::function<bool(sf::Packet&, PacketType)> onServerPacketReceived;

	void ReceivePackets(Client& client);
	void SendPackets(Client& client) const;

public:
	NetworkClientManager() = default;

	void SetOnMessageReceived(std::function<bool(sf::Packet&, PacketType)> onMessageReceived);
	void StartThreads(Client& client);
};