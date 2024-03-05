#include "ClientManager.h"

#include "PacketManager.h"

#include <thread>

ClientManager::~ClientManager()
{
	for (auto* c : clients)
	{
		delete c;
	}
}

bool ClientManager::isEmpty() const
{
	std::scoped_lock lock(mutex);
	return clients.empty();
}

std::size_t ClientManager::AddClient(sf::TcpSocket* socket)
{
	std::scoped_lock lock(mutex);

	// Search for a null client
	for (auto i = 0; i < clients.size(); i++)
	{
		if (clients[i] == nullptr)
		{
			clients[i] = new Client();
			clients[i]->socket = socket;
			return i;
		}
	}

	// If no null client is found, create a new one
	auto* c = new Client();
	c->socket = socket;
	clients.push_back(c);

	return clients.size() - 1;
}

void ClientManager::RemoveClient(std::size_t index)
{
	std::scoped_lock lock(mutex);
	delete clients[index];
	clients[index] = nullptr;
}

void ClientManager::CheckPacketToBeSent()
{
	for (auto* client : clients)
	{
		if (client == nullptr) continue;
		if (client->IsPacketsEmpty()) continue;

		std::scoped_lock lock(mutex);

		if (client->packetsToBeSent.empty()) continue;

		auto* packet = client->packetsToBeSent.front();

		PacketManager::SendPacket(*client->socket, packet);
		client->packetsToBeSent.pop();
		delete packet;
	}
}

void ClientManager::SendPacketToAllClients(Packet* packet, sf::TcpSocket* sender)
{
	for (auto* c : clients)
	{
		if (c != nullptr && c->socket != sender)
		{
			c->SendPacket(packet);
		}
	}
}

Client* ClientManager::operator[](std::size_t index)
{
	std::scoped_lock lock(mutex);
	return clients[index];
}