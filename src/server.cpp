#include "server.h"

#include "logger.h"

#include <thread>
#include <queue>
#include <mutex>

Server::~Server()
{
	running = false;
	listener.close();
}

bool Server::Bind(unsigned short port)
{
	return listener.listen(port) == sf::Socket::Done;
}

void Server::StartThreads()
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

void Server::SendMessageToAllClients(const MessagePacket& message, sf::TcpSocket* sender)
{
	clients.SendPacketToAllClients(PacketManager::CreatePacket(message), sender);
}

void Server::ReceivePacketFromClient(std::size_t clientIndex)
{
	bool receiving = true;
	auto* client = clients[clientIndex];
	sf::TcpSocket* socket = client->socket;
	while (receiving)
	{
		// Receive a message from the client
		sf::Packet answer;
		PacketType packetType = PacketManager::ReceivePacket(*socket, answer);

		if (packetType == PacketType::Invalid)
		{
			LOG_ERROR("Could not receive packet from client");
			break;
		}

		switch (packetType)
		{
		case PacketType::Connect:
		{
			ConnectPacket connectPacket = PacketManager::GetConnectPacket(answer);
			LOG("New connection received from " << connectPacket.playerName);

			// Send a message to all clients
			MessagePacket messageSent;
			messageSent.playerName = "Server";
			messageSent.message = connectPacket.playerName + " has joined the server";

			SendMessageToAllClients(messageSent, socket);
			client->packetsToBeSent.push(PacketManager::CreatePacket(AcknowledgementPacket()));
			break;
		}
		case PacketType::Disconnect:
		{
			DisconnectPacket disconnectPacket = PacketManager::GetDisconnectPacket(answer);
			LOG("Connection lost from " << disconnectPacket.playerName);
			receiving = false;

			// Send a message to all clients
			MessagePacket messageSent;
			messageSent.playerName = "Server";
			messageSent.message = disconnectPacket.playerName + " has left the server";

			SendMessageToAllClients(messageSent, socket);
			client->packetsToBeSent.push(PacketManager::CreatePacket(AcknowledgementPacket()));
			break;
		}
		case PacketType::Message:
		{
			MessagePacket messageReceived = PacketManager::GetMessagePacket(answer);
			LOG("Message received from " << messageReceived.playerName << ": " << messageReceived.message);

			// Send a message to all clients
			SendMessageToAllClients(messageReceived, socket);
			client->packetsToBeSent.push(PacketManager::CreatePacket(AcknowledgementPacket()));
			break;
		}
		case PacketType::Acknowledgement:
		{
			clients.Acknowledge(clientIndex);
			break;
		}
		default:
			LOG_ERROR("Unknown packet type received");
			break;
		}
	}

	clients.RemoveClient(clientIndex);
}