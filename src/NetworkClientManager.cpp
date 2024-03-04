#include "NetworkClientManager.h"

#include "PacketManager.h"
#include "Logger.h"

void NetworkClientManager::ReceivePackets(Client& client)
{
	while (running)
	{
		Packet* packet = PacketManager::ReceivePacket(*client.socket);

		if (packet->type == PacketType::Invalid)
		{
			LOG_ERROR("Could not receive packet");
			std::exit(EXIT_FAILURE);
		}

		if (onServerPacketReceived)
		{
			running = onServerPacketReceived(*packet);
		}

		delete packet;
	}
}

void NetworkClientManager::SendPackets(Client& client) const
{
	while (running)
	{
		if (client.packetsToBeSent.empty()) continue;

		auto* packet = client.packetsToBeSent.front();
		auto* sfPacket = PacketManager::CreatePacket(packet);
		client.packetsToBeSent.pop();
		client.socket->send(*sfPacket);
		delete packet;
		delete sfPacket;
	}
}

void NetworkClientManager::SetOnMessageReceived(const std::function<bool(const Packet&)>& onMessageReceived)
{
	this->onServerPacketReceived = onMessageReceived;
}

void NetworkClientManager::StartThreads(Client& client)
{
	std::thread receiveThread(&NetworkClientManager::ReceivePackets, this, std::ref(client));
	receiveThread.detach();

	std::thread sendThread(&NetworkClientManager::SendPackets, this, std::ref(client));
	sendThread.detach();
}
