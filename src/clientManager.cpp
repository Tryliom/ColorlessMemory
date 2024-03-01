#include "clientManager.h"

#include "packetManager.h"
#include "logger.h"

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

void ClientManager::Acknowledge(std::size_t index)
{
	std::scoped_lock lock(mutex);
	clients[index]->acknowledged = true;
}

void ClientManager::CheckPacketToBeSent()
{
	for (auto* client : clients)
	{
		std::scoped_lock lock(mutex);

		if (client == nullptr) continue;
		if (client->packetsToBeSent.empty() && client->packetWaitingForAcknowledgement == nullptr) continue;

		if (client->acknowledged)
		{
			auto* packet = client->packetsToBeSent.front();

			if (client->socket->send(*packet) == sf::Socket::Done)
			{
				client->packetsToBeSent.pop();
				client->ackClock.restart();

				// If the packet is a AcknowledgementPacket, do not wait for an acknowledgement
				if (PacketManager::GetPacketType(*packet) == PacketType::Acknowledgement)
				{
					LOG("Acknowledgement packet sent");

					client->acknowledged = true;
					delete packet;
					delete client->packetWaitingForAcknowledgement;
					client->packetWaitingForAcknowledgement = nullptr;
					continue;
				}

				client->acknowledged = false;
				client->packetWaitingForAcknowledgement = packet;
			}
			else
			{
				LOG_ERROR("Could not send packet to client");
			}
		}
		else if (client->ackClock.getElapsedTime().asMilliseconds() > Client::ACK_TIMEOUT)
		{
			// Resend the packet
			if (client->socket->send(*client->packetWaitingForAcknowledgement) == sf::Socket::Done)
			{
				client->ackClock.restart();
			}
			else
			{
				LOG_ERROR("Could not resend packet to client");
			}
		}
	}
}

void ClientManager::SendPacketToAllClients(sf::Packet* packet, sf::TcpSocket* sender)
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