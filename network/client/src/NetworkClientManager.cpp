#include "../include/NetworkClientManager.h"

#include "../../common/include/PacketManager.h"
#include "Logger.h"

#include <thread>

void NetworkClientManager::ReceivePackets(Client& client)
{
	while (_running)
	{
		Packet* packet = PacketManager::ReceivePacket(*client.socket);

		if (packet->Type == PacketType::Invalid)
		{
			LOG_ERROR("Could not receive packet");
			std::exit(EXIT_FAILURE);
		}

		std::scoped_lock lock(_mutex);
		_packetReceived.push(packet);
	}
}

void NetworkClientManager::SendPackets(Client& client) const
{
	while (_running)
	{
		//TODO: Correct datarace
		if (client.packetsToBeSent.empty()) continue;

		auto* packet = client.packetsToBeSent.front();
		auto* sfPacket = PacketManager::CreatePacket(packet);
		client.packetsToBeSent.pop();
		client.socket->send(*sfPacket);
		delete packet;
		delete sfPacket;
	}
}

void NetworkClientManager::StartThreads(Client& client)
{
	std::thread receiveThread(&NetworkClientManager::ReceivePackets, this, std::ref(client));
	receiveThread.detach();

	std::thread sendThread(&NetworkClientManager::SendPackets, this, std::ref(client));
	sendThread.detach();
}

Packet* NetworkClientManager::PopPacket()
{
	std::scoped_lock lock(_mutex);
	if (_packetReceived.empty()) return nullptr;

	auto* packet = _packetReceived.front();
	_packetReceived.pop();

	return packet;
}