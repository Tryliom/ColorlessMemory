#include "network_server_manager.h"

#include "logger.h"

#include <thread>
#include <queue>
#include <mutex>
#include <utility>

NetworkServerManager::~NetworkServerManager()
{
	running = false;
	listener.close();
}

bool NetworkServerManager::Bind(unsigned short port)
{
	return listener.listen(port) == sf::Socket::Done;
}

void NetworkServerManager::ListenToClientPackets(std::function<bool(sf::TcpSocket*, Packet*)> onMessageReceived)
{
	this->onClientMessageReceived = std::move(onMessageReceived);
}

void NetworkServerManager::StartThreads()
{
	std::thread clientAcceptor = std::thread([this]() {
		while (running)
		{
			// Accept a new connection
			std::scoped_lock lock(mutex);
			auto* socket = new sf::TcpSocket();

			if (listener.accept(*socket) != sf::Socket::Done)
			{
				LOG_ERROR("Could not accept connection");
			}
			else
			{
				auto index = clients.AddClient(socket);
				auto& client = *clients[index];

				LOG("Client connected: " << client.socket->getRemoteAddress() << ':' << client.socket->getRemotePort());

				// Start a new thread to receive packets from the client
				std::thread clientReceiver = std::thread([this, index]() {ReceivePacketFromClient(index);});
				clientReceiver.detach();
			}
		}
	});
	clientAcceptor.detach();

	std::thread packetSender = std::thread([&] {
		while (running)
		{
			if (clients.isEmpty()) continue;

			clients.CheckPacketToBeSent();
		}
	});
	packetSender.detach();
}

void NetworkServerManager::SendMessageToAllClients(MessagePacket* message, sf::TcpSocket* sender)
{
	clients.SendPacketToAllClients(message, sender);
}

void NetworkServerManager::ReceivePacketFromClient(std::size_t clientIndex)
{
	bool receiving = true;
	auto* client = clients[clientIndex];
	sf::TcpSocket* socket = client->socket;
	while (receiving)
	{
		// Receive a message from the client
		Packet* packet = PacketManager::ReceivePacket(*socket);

		if (packet->type == PacketType::Invalid)
		{
			LOG_ERROR("Could not receive packet from client");
			break;
		}

		if (onClientMessageReceived)
		{
			receiving = onClientMessageReceived(socket, packet);
		}

		delete packet;
	}

	clients.RemoveClient(clientIndex);
}